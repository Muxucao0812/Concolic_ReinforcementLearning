#include <cstdio>
#include "smt_lib.h"
#include "concolic.h"
#include <cstring>
#include <map>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <list>
#include <queue>
#include <vector>
#include <valarray>
#include <iostream>

using namespace std;

queue<SMTBasicBlock*> que_for_update_edge;

//-------------------------------Utilities -------------------------------------
static inline bool skip_assign(const string &str) {
	if (str.find(g_reset_sig_name) != string::npos) {
		return true;
	}
	return false;
}

char* smt_zeros(uint width){
    char* str = new char[width+3];
    memset(str, 0x30303030, width + 2);
    str[width+2] = 0;
    str[1] = 'b';
    return str;
}

static bool check_assign_dependency(SMTExpr* expr){
	SMTSignal* sig = dynamic_cast<SMTSignal*>(expr);
	if(sig){
		if(sig->parent->is_dep){
			return true;
		}
	}
	else{
		for(auto it:(expr->expr_list)){
			if(check_assign_dependency(it)){
				return true;
			}
		}
	}
	return false;
}

void add_to_rval(set<SMTSigCore*> &assign_set, SMTExpr* expr){
	SMTSignal* sig = dynamic_cast<SMTSignal*>(expr);
	if(sig){
		assign_set.insert(sig->parent);
	}
	else{
		for(auto it:(expr->expr_list)){
			add_to_rval(assign_set, it);
		}
	}
}

void smt_yices_assert(context_t *ctx, term_t term, SMTAssign* assign){
	yices_assert_formula(ctx, term);
	// if(enable_yices_debug){
	// 	if(assign){
	// 		//printf(";%s", assign->print(SMT_CLK_CURR).c_str());
	// 		yices_pp_term(stdout, term, 0xFFFFFF, 1, 0);
	// 	}
	// }
}

//static uint level = 0;

void smt_yices_push(){
	assert(yices_push(yices_context) == 0);
	/*if(enable_yices_debug){
		printf(";---------[PUSH %u]---------\n", level++);
	}*/
}

void smt_yices_pop(){
	assert(yices_pop(yices_context) == 0);
	/*if(enable_yices_debug){
		printf(";---------[POP %u]----------\n", --level);
	}*/
}



//----------------------------SMT Expr------------------------------------------
std::vector<SMTExpr*>  SMTExpr::all_expr_list;
SMTExpr::SMTExpr(const SMTExprType _type) : type(_type) {
	all_expr_list.push_back(this);
	is_bool = false;
	is_inverted = false;
	yices_term = NULL_TERM;
	is_term_eval_needed = true;
}

SMTExpr::~SMTExpr() {
	
}

void SMTExpr::add(SMTExpr* expr) {
	expr_list.push_back(expr);
}

SMTExpr* SMTExpr::get_child(uint idx) {
    if(idx < expr_list.size()){
        return expr_list[idx];
    }
    return NULL;
}

void SMTExpr::free_all() {
	while(!all_expr_list.empty()){
		delete all_expr_list.back();
		all_expr_list.back() = NULL;
		all_expr_list.pop_back();
	}
}



//------------------------------SMT String--------------------------------------
SMTString::SMTString(const char* str) :  SMTExpr(SMT_EXPR_STRING){
	_string = str;
}

void SMTString::print(std::stringstream& ss) {
	ss << _string;
}

term_t SMTString::eval_term(SMTClkType clk) {
	yices_term = NULL_TERM;
	is_term_eval_needed = false;
	return NULL_TERM;
}

SMTExpr* SMTString::get_expanded() {
	return this;
}

//----------------------------SMT Unspecified-----------------------------------
SMTUnspecified::SMTUnspecified() : SMTExpr(SMT_EXPR_UNSPECIFIED){
}

void SMTUnspecified::print(std::stringstream& ss) {
	ss << "_UNSPECIFIED_";
    error("Unspecified");
}

term_t SMTUnspecified::eval_term(SMTClkType clk) {
	yices_term = NULL_TERM;
	is_term_eval_needed = false;
	return NULL_TERM;
}

SMTExpr* SMTUnspecified::get_expanded() {
	return this;
}


//----------------------------SMT Concat----------------------------------------
SMTConcat::SMTConcat() : SMTExpr(SMT_EXPR_CONCAT){
	repeat = 1;
}

void SMTConcat::print(std::stringstream &ss) {
	const uint count = expr_list.size();
	if(repeat != 1){
		ss << "(bv-repeat ";
	}
	if(count > 1){
		ss << "(bv-concat";
		for(uint i=0; i<count; i++){
			if(expr_list[i] == NULL){
				error("Concatenation failed");
			}
			ss << " ";
			expr_list[i]->print(ss);
		}
		ss << ")";
	}
	else{
		expr_list[0]->print(ss);
	}
	if(repeat != 1){
		ss << " " << repeat << ")";
	}
}

term_t SMTConcat::eval_term(SMTClkType clk) {
	if(is_term_eval_needed){
		is_term_eval_needed = false;
		const uint size = expr_list.size();
		term_t* arr = new term_t[size];
		for(uint i = 0; i < size; i++){
			arr[i] = expr_list[i]->eval_term(clk);
			if(expr_list[i]->is_term_eval_needed){
				is_term_eval_needed = true;
			}
		}
		yices_term = yices_bvconcat(size, arr);
		if(repeat != 1){
			yices_term = yices_bvrepeat(yices_term, repeat);
		}
		delete [] arr;
	}
	return yices_term;
}

SMTExpr* SMTConcat::get_expanded() {
	SMTConcat* expr = new SMTConcat();
	expr->repeat = repeat;
	for(auto it:expr_list){
		expr->expr_list.push_back(it->get_expanded());
	}
	return expr;
}

//-----------------------------SMT Array---------------------------------------
SMTArray::SMTArray() : SMTExpr(SMT_EXPR_ARRAY){
	is_index_term = false;
}
SMTExpr* SMTArray::get_expanded() { return this;}
term_t SMTArray::eval_term(SMTClkType clk) {
	yices_term = parent->get_term(clk);
	// yices_pp_term(stdout, yices_term, 1000, 1, 0);
	if(is_index_term){
		return yices_application1(yices_term, index_term->get_term(SMT_CLK_CURR));
	}else{
		return yices_application1(yices_term, yices_bvconst_uint64(this->parent->index_width, array_index));
	}
	
	return yices_term;
}
term_t SMTArray::update_term() {
	this->parent->next_version++;
	term_t new_term = yices_new_uninterpreted_term(this->parent->bv_type);
	yices_set_term_name(new_term, (this->parent->name + string("_") + to_string(this->parent->next_version)).c_str());
	yices_term = new_term;
	this->parent->term_stack.push_back(new_term);
	return yices_term;
}
void SMTArray::print(std::stringstream& ss) {
	if(is_index_term){
		ss << "( " << parent->name << " " << array_index_str << " )";
	}else{
		ss << parent->name << " " << array_index << " ";
	}
}


//-----------------------------SMT Cust-----------------------------------------
SMTCust::SMTCust(std::string _operand) : SMTExpr(SMT_EXPR_CUSTOM){
    operand = _operand;
}

void SMTCust::print(std::stringstream& ss) {
    if(expr_list.size()){
        ss << "(" << operand;
        for(auto it:expr_list){
            ss << " ";
			it->print(ss);
        }
        ss << ")";
    }
    else{
        ss << operand;
    }
}

term_t SMTCust::eval_term(SMTClkType clk) {
	if(is_term_eval_needed){
		if(operand == "and"){
			is_term_eval_needed = false;
			const uint size = expr_list.size();
			term_t* arr = new term_t[size];
			for(uint i = 0; i < size; i++){
				arr[i] = expr_list[i]->eval_term(clk);
				if(expr_list[i]->is_term_eval_needed){
					is_term_eval_needed = true;
				}
			}
			yices_term = yices_and(size, arr);
			delete [] arr;
		}
		else if(operand == "/="){
			yices_term = yices_neq(expr_list[0]->eval_term(clk), expr_list[1]->eval_term(clk));
			is_term_eval_needed = expr_list[0]->is_term_eval_needed | expr_list[1]->is_term_eval_needed;
		}
		else if(operand == "true"){
			is_term_eval_needed = false;
			yices_term = yices_true();
		}
		else{
			error("wrong SMTCust type");
		}
	}
	return yices_term;
}

SMTExpr* SMTCust::get_expanded() {
	SMTCust* expr = new SMTCust(operand);
	for(auto it:expr_list){
		expr->expr_list.push_back(it->get_expanded());
	}
	return expr;
}

//-----------------------------SMT Unary----------------------------------------
SMTUnary::SMTUnary() : SMTExpr(SMT_EXPR_UNARY) {
    opcode = "invalid";
    //opcode = SMT_INVALID;
	func = NULL;
}

/*SMTUnary::SMTUnary(SMTUnaryOpcode opcode) {
    set_opcode(opcode);
}*/

void SMTUnary::set_opcode(char ivl_code) {
	switch (ivl_code) {
		case '-': opcode = "bv-neg";
            //set_opcode(SMT_BV_NOT);
			func = yices_bvneg;
			break;
		case '~': opcode = "bv-not";
            //set_opcode(SMT_BV_NOT);
			func = yices_bvnot;
			break;
		case '&': opcode = "bv-redand";
            //set_opcode(SMT_RE_AND);
			func = yices_redand;
			break;
		case '|': opcode = "bv-redor";
            //set_opcode(SMT_RE_OR);
			func = yices_redor;
			break;
		case '^': 
			error("TODO: add support for unary \'^\'");
			break;
		case 'A': opcode = "bv-redand";
			//info("inverted bv-redand used");
			func = yices_redand;
			is_inverted = true;
			break;
        case '!':                               //converted from bool to bitvector
		case 'N': opcode = "bv-redor";
			//info("inverted bv-redor used");
			func = yices_redor;
			is_inverted = true;
			break;
		case 'X': 
			error("TODO: add support for unary \'^\'");
            is_inverted = true;
			break;
		default: 
			;
	}
}

void SMTUnary::set_opcode(const ivl_expr_t expr) {
	const char ivl_code = ivl_expr_opcode(expr);
	set_opcode(ivl_code);
	if(opcode == "invalid"){
		error("Unsupported unary opcode for conversion: %c (%s:%u)", ivl_code, ivl_expr_file(expr), ivl_expr_lineno(expr));
	}
}

void SMTUnary::print(std::stringstream& ss) {
	assert(expr_list.size() == 1);

	//print
	if(is_inverted){
		ss << "(bv-not ";
	}
	ss << "(" << opcode << " ";
	expr_list[0]->print(ss);
	ss << ")";
	if(is_inverted){
		ss << ")";
	}
}

term_t SMTUnary::eval_term(SMTClkType clk) {
	if(is_term_eval_needed){
		yices_term = func(expr_list[0]->eval_term(clk));
		is_term_eval_needed = expr_list[0]->is_term_eval_needed;
		if(is_inverted){
			yices_term = yices_bvnot(yices_term);
		}
	}
	return yices_term;
}

SMTExpr* SMTUnary::get_expanded() {
	SMTUnary* expr = new SMTUnary();
	expr->opcode = opcode;
	expr->func = func;
	expr->expr_list.push_back(expr_list[0]->get_expanded());
	return expr;
}


//----------------------------SMT Binary----------------------------------------
SMTBinary::SMTBinary() : SMTExpr(SMT_EXPR_BINARY) {
	opcode = "invalid";
	func = NULL;
	is_signed = false;
}

void SMTBinary::set_opcode(char ivl_code) {
	switch (ivl_code) {
		case '%':
			opcode = "bv-rem";
			func = yices_bvsrem;
			break;
		case '/':
			opcode = "bv-div";
			func = yices_bvsdiv;
			break;
		case '+':
			opcode = "bv-add";
			func = yices_bvadd;
			break;
		case '-':
			opcode = "bv-sub";
			func = yices_bvsub;
			break;
		case '*':
			opcode = "bv-mul";
			func = yices_bvmul;
			break;
		case 'p':
			opcode = "bv-pow";
			// func = yices_bvpower;
			break;
        case 'n':
			opcode = "bv-comp";				//converted from bool to bitvector
			func = yices_redcomp;
            is_inverted = true;
			break;
		case 'e':
			opcode = "bv-comp";				//converted from bool to bitvector
			func = yices_redcomp;
			break;
		case '<':
			if(is_signed){
				opcode = "bv-slt";
				func = yices_bvslt_atom;
			}
			else{
				opcode = "bv-lt";
				func = yices_bvlt_atom;
			}
			is_bool = true;
			break;
		case 'L':
			if(is_signed){
				opcode = "bv-sle";
				func = yices_bvsle_atom;
			}
			else{
				opcode = "bv-le";
				func = yices_bvle_atom;
			}
			is_bool = true;		
			break;
		case '>':
			if(is_signed){
				opcode = "bv-sgt";
				func = yices_bvsgt_atom;
			}
			else{
				opcode = "bv-gt";
				func = yices_bvgt_atom;
			}
			is_bool = true;
			break;
		case 'G':
			if(is_signed){
				opcode = "bv-sge";
				func = yices_bvsge_atom;
			}
			else{
				opcode = "bv-ge";
				func = yices_bvge_atom;
			}
			is_bool = true;
			break;
		case '&':
			opcode = "bv-and";
			func = yices_bvand2;
			break;
		case '|':
			opcode = "bv-or";
			func = yices_bvor2;
			break;
		case '^':
			opcode = "bv-xor";
			func = yices_bvxor2;
			break;
		case 'A':
			opcode = "bv-nand";
			func = yices_bvnand;
			break;
		case 'O':
			opcode = "bv-nor";
			func = yices_bvnor;
			break;
		case 'X':
			opcode = "bv-xnor";
			func = yices_bvxnor;
			break;
		case 'a':
			opcode = "bv-and";
			func = yices_bvand2;
			break;      //converted from bool to bitvector
		case 'o':
			opcode = "bv-or";
			func = yices_bvor2;
			break;      //converted from bool to bitvector
		case 'l':
			opcode = "bv-shl";
			func = yices_bvshl;
			break;
		case 'r':
			opcode = "bv-lshr";
			func = yices_bvlshr;
			break;
		case 'R':
			opcode = "bv-ashr";
			func = yices_bvashr;
			break;

		default: 
			;	
	}
}

void SMTBinary::set_opcode_from_expr(const ivl_expr_t expr) {
	const char ivl_code = ivl_expr_opcode(expr);
	set_opcode(ivl_code);
	if(opcode == "invalid") {
		error("Unsupported binary opcode for conversion: %c (%s:%u)", ivl_code, ivl_expr_file(expr), ivl_expr_lineno(expr));
	}
}

void SMTBinary::print(std::stringstream& ss) {
	assert(expr_list.size() == 2);
		
	if(is_inverted){ ss << "(bv-not "; }
	if(is_bool){ ss << "(bool-to-bv "; }
	ss << "(" << opcode << " ";
	expr_list[0]->print(ss);
	ss << " ";
	expr_list[1]->print(ss);
	ss << ")";
	if(is_bool){ ss << ")"; }
	if(is_inverted){ ss << ")"; }
}

term_t SMTBinary::eval_term(SMTClkType clk) {
	if(is_term_eval_needed){	
		yices_term = func(expr_list[0]->eval_term(clk), expr_list[1]->eval_term(clk));
		// yices_pp_term(stdout, yices_term, 1000, 1, 0);
		// yices_print_error(stdout);
		if(is_bool){
			term_t p[1];
			p[0] = yices_term;
			yices_term = yices_bvarray(1, p);
		}
		if(is_inverted){
			yices_term = yices_bvnot(yices_term);
		}
		is_term_eval_needed = expr_list[0]->is_term_eval_needed | expr_list[1]->is_term_eval_needed;
	}
	// yices_pp_term(stdout, yices_term, 1000, 1, 0);
	return yices_term;
}

SMTExpr* SMTBinary::get_expanded() {
	SMTBinary* expr = new SMTBinary();
	expr->opcode = opcode;
	expr->func = func;
	expr->is_signed = is_signed;
	expr->expr_list.push_back(expr_list[0]->get_expanded());
	expr->expr_list.push_back(expr_list[1]->get_expanded());
	return expr;
}


//----------------------------SMT Logic-----------------------------------------
SMTLogic::SMTLogic() : SMTExpr(SMT_EXPR_LOGIC) {
	opcode = 0;
	func = NULL;
}

void SMTLogic::set_opcode(char ivl_code, bool is_inverted) {
	opcode = ivl_code;
    SMTLogic::is_inverted = is_inverted;
}

string SMTLogic::get_opcode() {
	switch (opcode) {
		case '&': 
			if(is_inverted){
				func = yices_bvnand;
				return "bv-nand";
			}
			else{
				func = yices_bvand2;
				return "bv-and";
			}
			break;
		case '|': 
			if(is_inverted){
				func = yices_bvnor;
				return "bv-nor";
			}
			else{
				func = yices_bvor2;
				return "bv-or";
			}
			break;
		case '^': 
			if(is_inverted){
				func = yices_bvxnor;
				return "bv-xnor";
			}
			else{
				func = yices_bvxor2;
				return "bv-xor";
			}
			break;
		default: 
			error("Unknown opcode for SMTLogic");	
	}
    return NULL;
}

void SMTLogic::print(std::stringstream& ss) {
	//info("SMTLogic used. Implement invert inheritance to improve perf");
	assert(expr_list.size() == 2);
	ss << "(" << get_opcode();
	for(auto it:expr_list){
		ss << " ";
		it->print(ss);
	}
	ss << ")";
}

term_t SMTLogic::eval_term(SMTClkType clk) {
	if(is_term_eval_needed){
		yices_term = func(expr_list[0]->eval_term(clk), expr_list[1]->eval_term(clk));
		is_term_eval_needed = expr_list[0]->is_term_eval_needed | expr_list[1]->is_term_eval_needed;
	}
	return yices_term;
}

SMTExpr* SMTLogic::get_expanded() {
	SMTLogic* expr = new SMTLogic();
	expr->opcode = opcode;
	expr->func = func;
	expr->expr_list.push_back(expr_list[0]->get_expanded());
	expr->expr_list.push_back(expr_list[1]->get_expanded());
	return expr;
}


//----------------------------SMT Ternary----------------------------------------
SMTTernary::SMTTernary() : SMTExpr(SMT_EXPR_TERNARY) {
}

void SMTTernary::print(std::stringstream& ss) {
	assert(expr_list.size() == 3);
    
	ss << "(ite (= ";
	expr_list[0]-> print(ss);
	ss << " 0b1) ";
	expr_list[1]->print(ss);
	ss << " ";
    expr_list[2]->print(ss);
	ss << ")";
}

term_t SMTTernary::eval_term(SMTClkType clk) {
	if(is_term_eval_needed){
		term_t cond = yices_eq(expr_list[0]->eval_term(clk), yices_bvconst_one(1));
		yices_term = yices_ite(cond, expr_list[1]->eval_term(clk), expr_list[2]->eval_term(clk));
		is_term_eval_needed = expr_list[0]->is_term_eval_needed | expr_list[1]->is_term_eval_needed | expr_list[2]->is_term_eval_needed;
	}
	return yices_term;
}

SMTExpr* SMTTernary::get_expanded() {
	SMTTernary* smt_ter = new SMTTernary();
	smt_ter->expr_list.push_back(expr_list[0]->get_expanded());
	smt_ter->expr_list.push_back(expr_list[1]->get_expanded());
	smt_ter->expr_list.push_back(expr_list[2]->get_expanded());
	return smt_ter;
}


//----------------------------SMT Assign----------------------------------------
vector<SMTAssign*> SMTAssign::assign_map;
SMTAssign::SMTAssign(SMTClkType _clk_type, SMTAssignType _assign_type,
		SMTExpr* _lval, SMTExpr* _rval, bool _is_commit):
			clk_type(_clk_type), assign_type(_assign_type), id(assign_map.size()),
			lval(_lval), rval(_rval), is_commit(_is_commit){
	covered_any_clock = false;
	yices_term = NULL_TERM;
	block = NULL;
	assign_map.push_back(this);
	process = SMTProcess::curr_proc;
	assert(process);
	is_first_print = true;
	is_first_print_exp = true;
	expanded_rval = NULL;
	expanded_lval = NULL;
	expanded_term = NULL_TERM;
	unexpanded_term = NULL_TERM;
}

void SMTAssign::init_assign() {
    instrument();
	partial_assign_check();
    SMTProcess::curr_proc->add_assign(this);
    SMTSigCore* lval_sig = get_lval_sig_core();
	lval_sig->assignments.push_back(this);
	if(rval->type != SMT_EXPR_NUMBER){
		//Yangdi:
		// lval_sig->is_state_variable = false;
	}
}


term_t SMTAssign::update_term() {
	SMTSignal* sig = dynamic_cast<SMTSignal*>(lval);
	if(sig){
		sig->parent->update_next_version();
		SMTSignal* tmp_sig = dynamic_cast<SMTSignal*>(lval);
		SMTNumber* tmp_num = dynamic_cast<SMTNumber*>(rval);
		if(tmp_sig&&tmp_num&&tmp_sig->is_index_term){
			int width = tmp_sig->parent->width;
			// term_t rval_term = rval->eval_term(SMT_CLK_CURR);
			term_t tmp_index = tmp_sig->_index->eval_term(SMT_CLK_CURR);
			term_t extend_tmp_index = yices_zero_extend(tmp_index, width-log2(width));
			term_t tmp_new_term = yices_new_uninterpreted_term(yices_bv_type(width));
			term_t mask = yices_bvshl(tmp_new_term,extend_tmp_index);
			yices_assert_formula(yices_context,yices_eq(tmp_new_term,yices_bvconst_int32(1,width)));
			if(tmp_num->get_value_bin()=="0"){
				yices_term = yices_bvand2(lval->eval_term(SMT_CLK_NEXT),yices_bvnot(mask));
			}else if(tmp_num->get_value_bin()=="1"){
				yices_term = yices_bvor2(lval->eval_term(SMT_CLK_NEXT),mask);
			}
		}else{
			yices_term = yices_eq(lval->eval_term(SMT_CLK_NEXT), rval->eval_term(SMT_CLK_CURR));
		}
		if(is_commit){
			sig->parent->commit();
		}
	}else{
		SMTArray *arr = dynamic_cast<SMTArray*>(lval);
		if(arr){
			yices_term = yices_eq(arr->update_term(),yices_update1(arr->parent->get_term(SMT_CLK_CURR),arr->_index->eval_term(SMT_CLK_CURR),rval->eval_term(SMT_CLK_CURR)));
			arr->parent->commit();
		}else{
			assert(0);
		}
	}
	return yices_term;
}


term_t SMTAssign::get_current_term() {
	if(unexpanded_term == NULL_TERM){
		unexpanded_term = yices_eq(lval->eval_term(SMT_CLK_CURR), rval->eval_term(SMT_CLK_CURR));
	}
	return unexpanded_term;
}

term_t SMTAssign::get_expanded_term() {
	//yices_pp_term(stdout, get_expanded_lval()->eval_term(SMT_CLK_CURR), 1000, 1, 0);
	if(expanded_term == NULL_TERM){
		expanded_term = yices_eq(get_expanded_lval()->eval_term(SMT_CLK_CURR), get_expanded_rval()->eval_term(SMT_CLK_CURR));
	}
	return expanded_term; 
}

term_t SMTAssign::update_expanded_term() {
	return yices_eq(get_expanded_lval()->eval_term(SMT_CLK_NEXT), get_expanded_rval()->eval_term(SMT_CLK_CURR));
}

bool SMTAssign::is_covered() {
	return covered_any_clock;
}

std::string SMTAssign::print() {
	if(is_first_print){
		is_first_print = false;
		ss << "(assert (= ";
		lval->print(ss);
		ss << "   ";
		rval->print(ss);
		ss << ")) ;" << id << "\n";
	}
	return ss.str();
}

std::string SMTAssign::print_expanded() {
	if(is_first_print_exp){
		is_first_print_exp = false;
		ss_exp << "(assert (= ";
		get_expanded_lval()->print(ss_exp);
		ss_exp << "   ";
		get_expanded_rval()->print(ss_exp);
		ss_exp << "));\n";
	}
	return ss_exp.str();
}

SMTExpr* SMTAssign::get_expanded_rval() {
	//expand rval if not already expanded
	if(!expanded_rval){
		expanded_rval = rval->get_expanded();
	}
	return expanded_rval;
}

SMTExpr* SMTAssign::get_expanded_lval() {
	//expand rval if not already expanded
	if(!expanded_lval){
		expanded_lval = lval->get_expanded();
	}
	return expanded_lval;
}

void SMTAssign::add_to_cont() {
	get_lval_sig_core()->cont_assign = this;
}

void SMTAssign::set_covered(uint sim_num) {
    if(covered_any_clock == false){
        covered_any_clock = true;
        covered_in_sim = sim_num;
    }
}

void SMTAssign::partial_assign_check() {
	SMTSignal* sig = dynamic_cast<SMTSignal*>(lval);
	if(!sig){
		SMTArray *arr = dynamic_cast<SMTArray*>(lval);
		if(arr){
			return ;
		}else{
			assert(false);
		}
	}
	SMTSigCore* parent = sig->parent;
	if((sig->msb - sig->lsb) != (parent->width - 1)){
		//error("Part select lval for assignment: %s", parent->name.c_str());
	}
}

SMTSigCore* SMTAssign::get_lval_sig_core() {
    SMTSignal* lval_sig = dynamic_cast<SMTSignal*>(lval);
	if(!lval_sig){
		SMTArray* arr = dynamic_cast<SMTArray*>(lval);
		if(arr){
			return arr->parent;
		}else{
			assert(false);
		}
	}
    return lval_sig->parent;
}

SMTAssign* SMTAssign::get_assign(uint id) {
	assert(id < assign_map.size());
    return assign_map[id];
}

uint SMTAssign::get_assign_count() {
	return assign_map.size();
}

void SMTAssign::instrument() {
	const string &str = print();
    if(skip_assign(str)){
		this->set_covered(0);
        return;
    }
	fprintf(g_out, " $display(\";A %u\");\t\t//%s", id, str.c_str());
	conc_flush(g_out);
}

void SMTAssign::print_coverage(std::ofstream& report) {
    //report << "      BRANCH |         SIM |\n";
    //report << "----------------------------\n";
    //puts("\n------------------------------------REPORT------------------------------------");
	puts("\n");
	uint total_stmt = 0;
	uint total_branch = 0;
	uint covered_stmt = 0;
	uint covered_branch = 0;
	const uint count = assign_map.size();
	for(uint i=0; i<count; i++){
		SMTAssign* assign = assign_map[i];
		switch(assign->assign_type){
			case SMT_ASSIGN_BLOCKING:
			case SMT_ASSIGN_NON_BLOCKING:
				total_stmt++;
				if(assign->is_covered()){
					covered_stmt++;
				}
				break;
			case SMT_ASSIGN_BRANCH:
				total_branch++;
				if(assign->is_covered()){
					covered_branch++;
                    report << setw(12) << assign->id << " ";
                    report << setw(12) << assign->covered_in_sim << " \n";
				}
				else{
					printf("[UNC BRANCH] %s", assign->print().c_str());
                    report << setw(12) << assign->id << " ";
                    report << setw(12) << "UNC" << " \n";
				}
				break;
			default:
				error("Invalid assign type");
		}
	}
	printf("[TOTAL BRANCH]   %u\n", total_branch);
	printf("[COVERED BRANCH] %u\n", covered_branch);
}

//------------------------SMT Blocking Assign-----------------------------------
SMTBlockingAssign::SMTBlockingAssign(SMTExpr* lval, SMTExpr* rval) : 
	SMTAssign(SMT_CLK_CURR, SMT_ASSIGN_BLOCKING, lval, rval, true){
    init_assign();
}

//----------------------SMT Non Blocking Assign---------------------------------
SMTNonBlockingAssign::SMTNonBlockingAssign(SMTExpr* lval, SMTExpr* rval) :
	SMTAssign(SMT_CLK_NEXT, SMT_ASSIGN_NON_BLOCKING, lval, rval, false) {
    init_assign();
}

//-----------------------------SMT Branch---------------------------------------
uint SMTBranch::total_branch_count;
uint SMTBranch::covered_branch_count=0;
uint SMTBranch::target_count;
uint SMTBranch::saved_total_branch;
uint SMTBranch::saved_covered_branch;
vector<SMTBranch*> SMTBranch::all_branches_list;
SMTBranch::SMTBranch(SMTBranchNode* _parent_node, SMTBranchType type, SMTExpr* lval, SMTExpr* rval) :
		SMTAssign(SMT_CLK_CURR, SMT_ASSIGN_BRANCH, lval, rval, false), 
        type(type),
		list_idx(_parent_node->branch_list.size()),
		parent_node(_parent_node){
	coverage = new bool[g_unroll+10];
	memset(coverage, false, sizeof(bool)*(g_unroll+10));
	parent_node->branch_list.push_back(this);
	covered_any_clock = false;
	k_permit_covered = 0;
	all_branches_list.push_back(this);
	is_dep = true;
}

bool SMTBranch::is_covered_clk(uint clock) {
	assert(clock <= g_unroll+10);
	return coverage[clock];
}

SMTBranch::~SMTBranch() {
	delete [] coverage;
}

void SMTBranch::set_covered_clk(uint sim_num, uint clock) {
	coverage[clock] = true;
	if(!is_covered()){
		set_covered(sim_num);
		covered_branch_count++;
		if(SMTBasicBlock::target_list.size() == 0){
			end_concolic();
		}
	}
}

void SMTBranch::clear_covered_clk(uint clock) {
    coverage[clock] = false;
}

/*void SMTBranch::update_distance() {
	if(!enable_cfg_directed){
		update_edge();
	}
    block->update_distance();
}*/

void SMTBranch::
get_state_variables(std::set<SMTSigCore*> &sigs, SMTExpr* expr) {
	SMTSignal* sig = dynamic_cast<SMTSignal*>(expr);
	if(sig){
		if(sig->parent->is_state_variable){
			sigs.insert(sig->parent);
		}
	}
	else{
		for(auto it:(expr->expr_list)){
			get_state_variables(sigs, it);
		}
	}
}

void SMTBranch::update_edge() {
	block->is_edge_updated = true;
	set<SMTSigCore*> sigs;
	get_state_variables(sigs, get_expanded_lval());
	if(sigs.size()){
		bool only_edge = true;
		//get expanded condition term and negate
		term_t not_cond_term = yices_not(get_expanded_term());
		for(auto a_sig:sigs){
			for(auto assign:a_sig->assignments){
				//get assignment term
				//term_t assign_term = assign->get_current_term();
				//Yangdi: use not branch & assign & branch
				term_t assign_term = assign->update_term();
				a_sig->commit();
				term_t cond_term = update_expanded_term();
				yices_reset_context(yices_context);
				// yices_pp_term(stdout, yices_and3(not_cond_term, assign_term, cond_term), 1000, 1, 0);
				yices_assert_formula(yices_context, yices_and3(not_cond_term, assign_term, cond_term));
				a_sig->restore_versions(0);
				if(yices_check_context(yices_context, NULL) == STATUS_SAT){
					if(only_edge){
						only_edge = false;
						//delete all predecessor edges
						block->predecessors.clear();
					}
					block->predecessors.insert(assign->block);
					if (!assign->block->is_edge_updated) {
						assign->block->is_edge_updated = true;
						que_for_update_edge.push(assign->block);
					}
				}
			}
		}
	}else if (block->idom && (!block->idom->is_edge_updated)){
		block->idom->is_edge_updated = true;
		que_for_update_edge.push(block->idom);
	}
}

// Xiangchen: Give every branch a random probability and the sum of all probabilities is 1
void SMTBranch::random_probability() {
    srand(time(NULL));
    double sum = 0.0;
    std::vector<double> probabilities;
    // 首先为每个分支生成一个随机数
    for ([[maybe_unused]] auto it : all_branches_list) {
        double prob = (double)(rand() % 1000) / 1000;
        probabilities.push_back(prob);
        sum += prob;
    }
    // 然后调整这些数，使它们的总和为1
    for (size_t i = 0; i < all_branches_list.size(); ++i) {
        all_branches_list[i]->branch_probability = probabilities[i] / sum;
        // printf("Branch %d probability: %f\n", all_branches_list[i]->id, all_branches_list[i]->branch_probability);
    }
}

// Xiangchen: Give every branch a probability according to the distance from the target
void SMTBranch::distance_probability() {
	double sum = 0.0;
	std::vector<double> probabilities;
	// 首先为每个分支生成一个随机数
	for (auto it : all_branches_list) {
		double prob = 1.0 / (it->block->distance + 1);
		probabilities.push_back(prob);
		sum += prob;
	}
	// 然后调整这些数，使它们的总和为1
	for (size_t i = 0; i < all_branches_list.size(); ++i) {
		all_branches_list[i]->branch_probability = probabilities[i] / sum;
		// printf("Branch %d probability: %f\n", all_branches_list[i]->id, all_branches_list[i]->branch_probability);
	}
}


// Xiangchen: Adjust every branch's probability according to if detect the new branch
// But if it can't detect the new branch, it will decrease the probability of the selected branch

void SMTBranch::increase_probability(SMTBranch* selected_branch) {
    double increase_factor = 0.005;
    selected_branch->branch_probability = std::min(1.0, selected_branch->branch_probability*(1+increase_factor));

    double total_probability = 0;
    for (auto branch : all_branches_list) {
        total_probability += branch->branch_probability;
    }

    for (auto branch : all_branches_list) {
        branch->branch_probability /= total_probability;
    }
}

void SMTBranch::decrease_probability(SMTBranch* selected_branch) {
    double decrease_factor = 0.01;
    selected_branch->branch_probability = std::max(0.0, selected_branch->branch_probability*(1 - decrease_factor));

    double total_probability = 0;
    for (auto branch : all_branches_list) {
        total_probability += branch->branch_probability;
    }

    for (auto branch : all_branches_list) {
        branch->branch_probability /= total_probability;
    }
}

void SMTBranch::print_probability() {
	FILE* g_prob = fopen(g_prob_file, "w");
	for (auto branch : all_branches_list) {
		fprintf(g_prob, "Branch %d probability: %f\n", branch->id, branch->branch_probability);
	}
	fclose(g_prob);
}



void SMTBranch::instrument() {
    const string &str = print();
	total_branch_count++;
    if(skip_assign(str)){
		covered_branch_count++;
		this->set_covered(0);
        return;
    }
	fprintf(g_out, "%*c$display(\";A %u\");\t\t//%s", get_indent(), ' ', id, str.c_str());
	//fprintf(g_out, "%*c//exp: %s", get_indent(), ' ', print_expanded().c_str());
	conc_flush(g_out);
}

SMTBranch* SMTBranch::create_case_branch(SMTBranchNode* parent, SMTExpr* case_expr) {
	return new SMTBranch(parent, SMT_BRANCH_CASE, parent->cond, case_expr);
}

SMTBranch* SMTBranch::_create_condit_branch(SMTBranchNode* parent, const char* num_expr) {
	if(parent->cond_width > 1){
		SMTUnary* un = new SMTUnary();
		un->set_opcode('|');
		un->add(parent->cond);
		parent->cond = un;
		parent->cond_width = 1;
	} 
	SMTNumber* smt_number = new SMTNumber(num_expr, 1, false);
	return new SMTBranch(parent, SMT_BRANCH_CONDIT, parent->cond, smt_number);
}

SMTBranch* SMTBranch::create_true_branch(SMTBranchNode* parent) {
	return _create_condit_branch(parent, "1");
}

SMTBranch* SMTBranch::create_false_branch(SMTBranchNode* parent) {
	return _create_condit_branch(parent, "0");
}

SMTBranch* SMTBranch::create_default_branch(SMTBranchNode* parent) {
    SMTExpr* smt_and = new SMTCust("and");
    SMTExpr* cond = parent->cond;
    const uint size = parent->branch_list.size();
    for(uint i = 0; i < size; i++){
        SMTExpr* smt_neq = new SMTCust("/=");
        smt_neq->add(cond);
        smt_neq->add(parent->branch_list[i]->rval);
        smt_and->add(smt_neq);
    }
    
	return new SMTBranch(parent, SMT_BRANCH_CASE, smt_and, new SMTCust("true"));
}

void SMTBranch::clear_k_permit() {
    for(auto it:all_branches_list){
		it->k_permit_covered = 0;
	}
}

void SMTBranch::clear_coverage(uint min_clock) {
    for(auto it:all_branches_list){
		for(uint clock = min_clock; clock <= g_unroll+10; clock++){
            it->coverage[clock] = false;
        }
	}
}

void SMTBranch::save_coverage() {
	saved_total_branch = total_branch_count;
	saved_covered_branch = covered_branch_count;
	for(auto it:all_branches_list){
		it->saved_coverage =  it->covered_any_clock;
	}
}

void SMTBranch::restore_coverage() {
	total_branch_count = saved_total_branch;
	covered_branch_count = saved_covered_branch;
	for(auto it:all_branches_list){
		it->covered_any_clock = it->saved_coverage;
	}
}

term_t SMTBranch::update_term() {
	//printf("%s", print(clk_type).c_str());
	yices_term = yices_eq(lval->eval_term(SMT_CLK_CURR), rval->eval_term(SMT_CLK_CURR));
	return yices_term;
}

//-------------------------SMT Branch Node--------------------------------------
SMTBranchNode::SMTBranchNode() {

}

//-------------------------SMT Branch Node Detected----------------------------
SMTBranchNodeCovered::SMTBranchNodeCovered() {
	
}

//-------------------------SMT Branch Case--------------------------------------
/*SMTBranchCase::SMTBranchCase() : SMTBranchNode(SMT_BRANCH_CASE){
	error("TODO: Branch case");
}

SMTBranch* SMTBranchCase::get_switch_cond() {
	error("TODO: Branch case");
    return NULL;
}*/


//----------------------------SMT Number----------------------------------------
const char hex_map[16] = {	'0', '1', '2', '3', '4', '5', '6', '7',
							'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

SMTNumber::SMTNumber(const char* bits, int bit_width, bool is_signed) : SMTExpr(SMT_EXPR_NUMBER) {
	has_undef = false;
    this->is_signed = is_signed;
    assert(bit_width);
	width = bit_width;
	value_hex = "";
    
	const int nbits = bit_width - 1;
	//print in binary
	value_bin = "";
	value_bin.reserve(width+1);
	for(int i = nbits; i>=0; i--){
		if((bits[i] != '0') && (bits[i] != '1')){
			value_bin += '0';
            has_undef = true;
            //info("Number expression has x or z or ?");
		}
		else{
			value_bin += bits[i];
		}
	}
    
    if(!(width & 3)){
        uint sum = 0;
        for(uint i=0; i<width; i++){
            uint offset = 3 - (i & 3);
            sum += (value_bin[i] - '0') << offset;
            if(offset == 0){
                value_hex += hex_map[sum];
                sum = 0;
            }
        }
    }
}

void SMTNumber::print(std::stringstream& ss) {
	if(width & 3){
        ss << "0b" << value_bin;
    }
    else{
        ss << "0h" << value_hex;
    }
}

void SMTNumber::emit_verilog_value() {
    if(is_signed){
        fprintf(g_out, "%u\'s", width);
    }
    else{
        fprintf(g_out, "%u\'", width);
    }
    
    if(width & 3){
        fprintf(g_out, "b%s", value_bin.c_str());
    }
    else{
        fprintf(g_out, "h%s", value_hex.c_str());
    }
}

term_t SMTNumber::eval_term(SMTClkType clk) {
	if(is_term_eval_needed){
		yices_term = yices_parse_bvbin(value_bin.c_str());
		is_term_eval_needed = false;
	}
	return yices_term;
}

bool SMTNumber::is_equal(SMTNumber* num) {
	return (value_bin == num->value_bin);
}

std::string SMTNumber::get_value_bin() {
	return value_bin;
}

SMTExpr* SMTNumber::get_expanded() {
	return this;
}

//---------------------------SMT Signal Core------------------------------------
std::unordered_map<ivl_signal_t, SMTSigCore*> SMTSigCore::sig_to_core_map;
vector<SMTSigCore*> SMTSigCore::reg_list;
vector<SMTSigCore*> SMTSigCore::input_list;


SMTSigCore::SMTSigCore(ivl_signal_t sig){
    name = ivl_signal_basename(sig);
    width = ivl_signal_width(sig);
    zeros = smt_zeros(width);


	is_hamming = true;
	sig_to_core_map[sig] = this;
    int msb;
    int lsb;
    get_sig_msb_lsb(sig, &msb, &lsb);
	if(msb < lsb){
        error("msb < lsb => currently not supported");
    }
    assert(width == (msb-lsb+1));
    if(ivl_signal_port(sig) != IVL_SIP_INPUT){
        reg_list.push_back(this);
		is_dep = false;
		is_state_variable = true;
		if(ivl_signal_dimensions(sig)){
			is_array = true;
		}
    } else{
        input_list.push_back(this);
		is_dep = true;
		is_state_variable = false;
    }
	cont_assign = NULL;
	curr_version = 0;
    next_version = 0;
	was_in_queue = false;

	//set width type
	int index_count = ivl_signal_array_count(sig);

	//if this signal is an not an array
	if(index_count == 1){
		//set width type
		bv_type = yices_bv_type(width);

		//create g_unroll number of terms
		for(uint i = 0; i <= g_unroll+10; i++){
			term_t new_term = yices_new_uninterpreted_term(bv_type);
			yices_set_term_name(new_term, (name + string("_") + to_string(i)).c_str());
			term_stack.push_back(new_term);
		}

		//create zero term
		init_term = yices_eq(term_stack[0], yices_bvconst_zero(width));
		version_at_clock.resize(g_unroll + 10);
		version_at_clock[0] = 0;
	}else{
		type_t bv_value_type = yices_bv_type(width);
		index_width = (int)log2(float(index_count));
		type_t bv_index_type = yices_bv_type(index_width);
		bv_type = yices_function_type1(bv_index_type,bv_value_type);

		//since array update with alias every time, we would not declare all terms at first

		term_t new_term = yices_new_uninterpreted_term(bv_type);

		yices_set_term_name(new_term, (name + string("_") + to_string(0)).c_str());
		term_stack.push_back(new_term);
		
		// for(auto it = term_stack.begin(); it != term_stack.end(); ++it){
		// 	yices_pp_term(stdout, *it, 1000, 1, 0);
		// }


		for(int i = 0;i < index_count;++i){
			term_t index_term = yices_bvconst_uint32(index_width, i);
			term_t value_term = yices_bvconst_zero(width);
			term_t eq_term = yices_eq(yices_application1(term_stack[0],index_term), value_term);
		
			// yices_pp_term(stdout, eq_term, 1000, 1, 0);
			if(i == 0){
				init_term = eq_term;
			}else{
				init_term = yices_and2(init_term, eq_term);
			}
		}
		if(init_term==-1){
			assert(false);
		}
		version_at_clock.resize(g_unroll + 10);
		version_at_clock[0] = 0;
	}
}

// {
//     name = ivl_signal_basename(sig);
//     width = ivl_signal_width(sig);
//     zeros = smt_zeros(width);
// 	sig_to_core_map[sig] = this;//Xiangchen: Index width is not right
//     int msb;
//     int lsb;
//     get_sig_msb_lsb(sig, &msb, &lsb);
//     if(msb < lsb){
//         error("msb < lsb => currently not supported");
//     }
//     assert(width == (msb-lsb+1));
//     if(ivl_signal_port(sig) != IVL_SIP_INPUT){
//         reg_list.push_back(this);
// 		is_dep = false;
// 		is_state_variable = true;
//     } else{
//         input_list.push_back(this);
// 		is_dep = true;
// 		is_state_variable = false;
//     }
// 	cont_assign = NULL;
// 	curr_version = 0;
//     next_version = 0;
// 	was_in_queue = false;
//     //set width type
//     bv_type = yices_bv_type(width);

//     //create g_unroll number of terms
//     for(uint i = 0; i <= g_unroll; i++){
//         term_t new_term = yices_new_uninterpreted_term(bv_type);
//         yices_set_term_name(new_term, (name + string("_") + to_string(i)).c_str());
//         term_stack.push_back(new_term);
//     }

//     //create zero term
//     init_term = yices_eq(term_stack[0], yices_bvconst_zero(width));
// 	version_at_clock.resize(g_unroll + 2);
// 	version_at_clock[0] = 0;
// }

SMTSigCore::~SMTSigCore() {
}

void SMTSigCore::update_next_version() {
    next_version++;
	
    if(next_version == term_stack.size()){
        term_t new_term = yices_new_uninterpreted_term(bv_type);
        yices_set_term_name(new_term, (name + string("_") + std::to_string(next_version)).c_str());
        term_stack.push_back(new_term);
    }
}

term_t SMTSigCore::get_term(SMTClkType clk) {
	uint version;
	//Yangdi: fix mem-ctrl problem
	//one block is executed twice in one clock
	//Assign 906 u5_mc_le <= 1'b0
	//Then A916 branch is executed if (!u5_mc_le <= 1'b0)
	// if (next_version > curr_version + 1) {
	// 	printf("current %d\t next %d\n", curr_version, next_version);
	// 	curr_version = next_version - 1;
	// }
		
	if(clk == SMT_CLK_CURR){
		version = curr_version;
	} else {
		version = next_version;
	}
	
	return term_stack[version];
}

void SMTSigCore::free_all() {
	for(auto it:reg_list){
		delete it;
	}
	reg_list.clear();
	for(auto it:input_list){
		delete it;
	}
	input_list.clear();
}

SMTSigCore* SMTSigCore::get_parent(ivl_signal_t sig) {
	return sig_to_core_map[sig];
}

void SMTSigCore::clear_all_versions() {
	for(auto it:reg_list){
		it->curr_version = 0;
		it->next_version = 0;
	}
    for(auto it:input_list){
        it->curr_version = 0;
        it->next_version = 0;
    }
}

void SMTSigCore::commit() {
    curr_version = next_version;
}

void SMTSigCore::commit_versions(uint clock) {
	for(auto it:reg_list){
		it->commit();
		it->version_at_clock[clock] = it->curr_version;
	}
}

void SMTSigCore::restore_versions(uint clock) {
	for(auto it:reg_list){
		it->curr_version = it->version_at_clock[clock];
		it->next_version = it->curr_version;
	}
	set_input_version(clock);
}

void SMTSigCore::yices_insert_reg_init(context_t* ctx) {
	for(auto it:reg_list){
		yices_assert_formula(ctx, it->init_term);
	}
	for(auto it:input_list){
		yices_assert_formula(ctx, it->init_term);
	}
}

void SMTSigCore::print_state_variables(std::ostream &out) {
	out << "//state variables:";
	for(auto it:reg_list){
		if(it->is_state_variable){
			out << ' ' << it->name;
		}
	}
    out << "\n\n";
}

void SMTSigCore::set_input_version(uint version) {
    for(auto it:input_list){
        it->curr_version = version;
    }
}


//----------------------------SMT Signal----------------------------------------
SMTSignal::SMTSignal() : SMTExpr(SMT_EXPR_SIGNAL) {
	is_index_term = false;
}

SMTSignal::SMTSignal(ivl_signal_t sig) : SMTExpr(SMT_EXPR_SIGNAL) {
	is_index_term = false;
    parent = SMTSigCore::get_parent(sig);
    get_sig_msb_lsb(sig, &msb, &lsb);
}

SMTExpr* SMTSignal::get_expanded() {
	if(parent->assignments.size() == 1){
		return parent->assignments[0]->get_expanded_rval();
	}
	else{
		return this;
	}
}

void SMTSignal::print(std::stringstream& ss) {
	if((msb - lsb) == (parent->width - 1)){
		ss << parent->name << ' ';
	}
	else{	//part select
		ss << "(bv-extract " << msb << " " << lsb << " " << parent->name << " )";
	}
}

term_t SMTSignal::eval_term(SMTClkType clk) {
	yices_term = parent->get_term(clk);
	// if(true){
	// 	cout<<"[PRINT TERM STACK]"<<endl;
	// 	for(int i = 0;i < parent->term_stack.size();++i){
	// 		yices_pp_term(stdout, parent->term_stack[i],80, 10, 0);
	// 	}
	// }
	// yices_pp_term(stdout, yices_term, 1000, 1, 0);
	if((msb - lsb) != (parent->width - 1) && !this->is_index_term){
		yices_term = yices_bvextract(yices_term, lsb, msb);
	}
	return yices_term;
}

//----------------------------SMT Process---------------------------------------
SMTProcess* SMTProcess::curr_proc;
vector<SMTProcess*> SMTProcess::process_list;
SMTProcess SMTProcess::g_comb_process;

SMTProcess::SMTProcess() {
    process_list.push_back(this);
	is_edge_triggered = false;
    entry_block = new SMTBasicBlock();
    exit_block = entry_block;
    top_bb = entry_block;
    is_expanded = false;
    expanding_now = false;
}

/*SMTProcess::SMTProcess(SMTProcess& obj) {
	process_list.push_back(this);
	is_edge_triggered = obj.is_edge_triggered;
    is_expanded = obj.is_expanded;
	expanding_now = obj.expanding_now;
	sensitivity_list = obj.sensitivity_list;
	signal_assign_list = obj.sig_assign_list;
	
	
    exit_block = NULL;
    top_bb = entry_block;
}*/


SMTProcess::~SMTProcess() {
    delete entry_block;
    entry_block = NULL;
}

void SMTProcess::add_assign(SMTAssign* assign) {
    top_bb->assign_list.push_back(assign);
    assign->block = top_bb;
}

void SMTProcess::expand() {
    /*if(expanding_now){
        error("Cyclic dependency");
    }
    expanding_now = true;
    if(is_expanded == false){
        if(sig_assign_blocks.size()){
            //Some value is assigned within this process. Need to expand.
            //Go through basic blocks having blocking assignments and expand.
            for(auto blk:sig_assign_blocks){
                const uint size = blk->assign_list.size();
                for(uint i = 0; i < size; i++){
                    SMTAssign* assign = blk->assign_list[i];
                    if(assign->assign_type == SMT_ASSIGN_BLOCKING){
                        SMTSigCore* sig = assign->get_lval_sig_core();
                        const uint size_dep = sig->dependent_process.size();
                        //pad expanded version of all the processes that are sensitive to this assign
                        if(size_dep){
                            sig->dependent_process[0]->expand();
                            SMTBasicBlock* entry_block = sig->dependent_process[0]->entry_block;
                            SMTBasicBlock* exit_block = sig->dependent_process[0]->exit_block;
                            for(uint i = 1; i < size_dep; i++){
                                SMTProcess* proc = sig->dependent_process[i];
                                proc->expand();
                                proc->entry_block->predecessors.push_back(exit_block);
                                exit_block->successors.push_back(proc->entry_block);
                                exit_block = proc->exit_block;
                            }
                        }
                    }
                }
            }
        }
        is_expanded = true;
    }
    expanding_now = false;*/
}

void SMTProcess::combine_processes() {
    //only need to combine processes when size is more than 1
    if(process_list.size() > 1){
        //first we need to expand all processes based on dependency
        for(auto it:process_list){
            it->expand();
        }
    }
}

void SMTProcess::make_circular() {
    for(auto it:process_list){
        if(it->entry_block != it->exit_block){
            if(it->entry_block->predecessors.size() == 0){
                it->entry_block->predecessors.insert(it->exit_block);
            }
        }
    }
}

//--------------------------SMT Basic Block-------------------------------------
uint SMTBasicBlock::id_counter = 0;
list<SMTBasicBlock*> SMTBasicBlock::target_list;
list<SMTBasicBlock*> SMTBasicBlock::covered_target_list;
list<SMTBasicBlock*> SMTBasicBlock::uncovered_target_list;
std::vector<SMTBasicBlock*> SMTBasicBlock::block_list;
SMTBasicBlock::SMTBasicBlock() : id(id_counter) {
    id_counter++;
    block_list.push_back(this);
	weight = 0;
	distance = initial_distance;
	idom = NULL;
	adjacency_list = NULL;
	is_edge_updated = false;
}

SMTBasicBlock::~SMTBasicBlock() {
	if(adjacency_list){
		delete adjacency_list;
		adjacency_list = NULL;
	}
}


/*SMTBasicBlock::SMTBasicBlock(SMTBasicBlock& obj) : id(id_counter) {
	id_counter++;
	block_list.push_back(this);
	assign_list = obj.assign_list;
	
}*/

void SMTBasicBlock::print_assigns(ofstream &out) {
    for(auto it:assign_list){
        out << it->print();
    }
}

void SMTBasicBlock::print(ofstream &out) {
    out << "[" << id << "] weight: " << weight << " distance: " << distance;
	if(idom){
		out <<  " idom: " << idom->id;
	}
	out << '\n';
    print_assigns(out);
    if(successors.size()){
        out << "[S]";
        for(auto it:successors){
            out << ' ' << it->id;
        }
        out << '\n';
    }
    if(predecessors.size()){
        out << "[P]";
        for(auto it:predecessors){
            out << ' ' << it->id;
        }
        out << '\n';
    }
    out << '\n';
}

// update distance of all blocks from current block
void SMTBasicBlock::update_distance() {
	SMTBasicBlock::reset_flags();	//reset distances of all blocks
	distance = 0;

	queue<SMTBasicBlock*> bb_queue;
	bb_queue.push(this);

	while(!bb_queue.empty()){

		SMTBasicBlock* top_bb = bb_queue.front();
		//printf("topbb: %u, precess: %lu, dist: %u\n", top_bb->id, top_bb->predecessors.size(), top_bb->distance);
		bb_queue.pop();

		const uint curr_dist = top_bb->distance;
		uint updated = curr_dist + top_bb->weight;

		for(auto it:top_bb->predecessors){
			//Yangdi: If the number of pre is small, they are important
			if(updated < it->distance){
				//printf("Updating its pre: %u\n", it->id);
				it->distance = updated;
				//it->distance = curr_dist + top_bb->weight;
				bb_queue.push(it);
			}
		}
		if (top_bb->idom && top_bb->idom->distance > curr_dist + top_bb->weight) {
			//printf("Updating its idom: %u\n", top_bb->idom->id);
			top_bb->idom->distance = curr_dist + top_bb->weight;
			bb_queue.push(top_bb->idom);
		}

	}
}



// update path
void SMTBasicBlock::update_path(SMTPath* path, const vector<constraint_t*> &constraints_stack) {
	SMTBasicBlock::reset_flags();	//reset distances of all blocks
	std::pair<uint, uint> dist_clock = distance_from_current_path(constraints_stack);
	closest_path = path;
	closest_path_distance = dist_clock.first;
	closest_path_clock = dist_clock.second;
}

void SMTBasicBlock::print_distance() {
	for(auto it:block_list){
		printf("Block %u: %u\n", it->id, it->distance);
	}
}

// update distance of all blocks from adjacent list
void SMTBasicBlock::update_distance_from_adjacency_list() {
	assert(adjacency_list);
	for(auto a_block:SMTBasicBlock::block_list){
		a_block->distance = adjacency_list[a_block->id];
	}
}

void SMTBasicBlock::update_edge() {
	if(assign_list.size()){
		if(assign_list[0]->assign_type == SMT_ASSIGN_BRANCH){
			SMTBranch* br = dynamic_cast<SMTBranch*>(assign_list[0]);
			br->update_edge();
		}
	} else if (idom && (!idom->is_edge_updated)) {
		idom->is_edge_updated = true;
		que_for_update_edge.push(idom);
	}
}


void SMTBasicBlock::print_all(ofstream &out) {
    out << "/*\n";
    for(auto it:block_list){
        it->print(out);
    }
    out << "*/\n\n";
}



void SMTBasicBlock::print_cover_result() {
	uint uncovered_targets_count = 0;
	uint covered_targets_count = 0;
    g_cover_result = fopen(g_cover_result_file, "w");

    if (g_cover_result == nullptr) {
        std::cerr << "Failed to open file: " << g_cover_result_file << std::endl;
        return;
    }

	fprintf(g_cover_result, "Uncovered targets:\n");
    for (auto it : SMTBasicBlock::uncovered_target_list) {
        if (it->assign_list.size() && !it->assign_list[0]->is_covered()) {
            fprintf(g_cover_result, "%d\n", it->assign_list[0]->id);
			uncovered_targets_count++;
        }
    }
	fprintf(g_cover_result, "Total uncovered targets number: %d\n", uncovered_targets_count);

	fprintf(g_cover_result, "Covered targets:\n");
	for (auto it : SMTBasicBlock::covered_target_list) {
		if (it->assign_list.size() && it->assign_list[0]->is_covered()) {
			fprintf(g_cover_result, "%d\n", it->assign_list[0]->id);
			covered_targets_count++;
		}
	}
	fprintf(g_cover_result, "Total covered targets number: %d\n", covered_targets_count);
    fclose(g_cover_result); // Close the file after writing
// 	printf("Covered targets: %d, Uncovered targets: %d, Coverage: %.2f%%\n", 
//        SMTBranch::target_count - uncovered_targets_count, 
//        uncovered_targets_count, 
//        100.0 * (double)(SMTBranch::target_count - uncovered_targets_count) / SMTBranch::target_count);
}


void SMTBasicBlock::reset_flags() {
    for(auto it:block_list){
        it->distance = initial_distance;
    }
}

void SMTBasicBlock::add_target(SMTBasicBlock* target) {
	target_list.push_back(target);
}

/*void SMTBasicBlock::remove_target(SMTBasicBlock* target) {
	target_list.erase(target);
}*/

void SMTBasicBlock::remove_covered_targets(uint iter) {
	std::list<SMTBasicBlock*>::iterator it = target_list.begin();
	while (it != target_list.end()) {
		if((*it)->assign_list.size() && (*it)->assign_list[0]->is_covered()){
			printf("[COVERED AT %d CLOCK] %s", iter, (*it)->assign_list[0]->print().c_str());
			SMTBasicBlock::covered_target_list.push_back(*it);
			it = target_list.erase(it);
		} else {
			++it;
		}
	}
}

/*void SMTBasicBlock::remove_dominator_targets() {
	for(auto it:target_list){
		//empty targets are already removed.
		assert(it->assign_list.size());
		SMTBasicBlock* curr_block = it->idom;
		while(curr_block){
			target_list.erase(curr_block);
			curr_block = curr_block->idom;
		}
    }
}*/

void SMTBasicBlock::edge_realignment() {
	for(auto it:target_list){
		it->is_edge_updated = true;
		que_for_update_edge.push(it);
	}
	while (!que_for_update_edge.empty()) {
		SMTBasicBlock *current = que_for_update_edge.front();
		que_for_update_edge.pop();
		current->update_edge();
	}
}

SMTBasicBlock* SMTBasicBlock::get_target() {
    if(!target_list.empty()){
        return target_list.front();
    }
    return NULL;
}

uint SMTBasicBlock::target_num() {
	return target_list.size();
}

void SMTBasicBlock::update_all_closest_paths(SMTPath* path, const vector<constraint_t*> &constraints_stack) {
	for(auto it:target_list){
		it->update_closest_path(path, constraints_stack);
	}
}


// void SMTBasicBlock::update_all_biggest_probability_paths(SMTPath* path, const vector<constraint_t*> &constraints_stack) {
// 	for(auto it:target_list){
// 		it->update_all_probabilities(path, constraints_stack);
// 	}
// }

// void SMTBasicBlock::update_all_probabilities(SMTPath* path, const vector<constraint_t*> &constraints_stack) {
// 	// for(auto it:target_list){
// 		// calculate the probability of each target
// 		// need to finish
// 		info("This function need to be finished");
// 	// }
// }


void SMTBasicBlock::update_closest_path(SMTPath* path, const vector<constraint_t*> &constraints_stack) {
	//Yangdi: 
	//if(closest_path_distance > 1){
	std::pair<uint, uint> dist_clock = distance_from_current_path(constraints_stack);
	//printf("id: %d, dist: %d, clock: %d\n", assign_list[0]->id, dist_clock.first, dist_clock.second);

	if ((dist_clock.first < closest_path_distance) || (dist_clock.first == closest_path_distance && dist_clock.second < closest_path_clock)){
		closest_path = path;
		closest_path_distance = dist_clock.first;
		closest_path_clock = dist_clock.second;
	}
	//printf("id: %d, min dist: %d, min clock: %d\n", assign_list[0]->id, closest_path_distance, closest_path_clock);
	//}
}

// Yangdi: return the distance and clock
std::pair<uint, uint> SMTBasicBlock::distance_from_current_path(const vector<constraint_t*> &constraints_stack) {
	uint min_dist = 0xFFFFFFF;
	uint min_clock = 0xFFFFFFF;
	for(auto it:constraints_stack){
		//if(it->obj && it->clock > 0){
		if(it->obj){
			uint dist = adjacency_list[it->obj->block->id];
			if(dist < min_dist || ((dist == min_dist) && (it->clock < min_clock))){
			//if(dist < min_dist){
				min_dist = dist;
				min_clock = it->clock;
			}
		}
	}

	return std::make_pair(min_dist, min_clock);
}

void SMTBasicBlock::dump_distances() {
	for(auto a_block:block_list){
		adjacency_list[a_block->id] = a_block->distance;
	}
}

void SMTBasicBlock::update_all_distances() {
	for(auto it:target_list){
		//calculate the relative distances from current target
		it->update_distance();
		
		//save the distances in adjacency list
		it->adjacency_list = new uint[id_counter];
		for(auto a_block:block_list){
			it->adjacency_list[a_block->id] = a_block->distance;
		}
	}
}

//--------------------------------SMT Path--------------------------------------
SMTPath::SMTPath(CTDataMem& curr_data) {
    data = curr_data;
}


SMTPath::~SMTPath() {	

}
void SMTPath::Dump(const char* file) {
	// data dump into g_data_mem
	this->data.dump(file);

}
	

// void SMTPath::UpdatePath() {
// 	// add the data_step into data
// 	this -> data_step.in_ports = this -> data.in_ports;
//     this->data_step.set_width(this->data.get_width());
//     this->data_step.set_clk(this->data.get_step() + this->data.get_clk());

//     for (uint i = 0; i < this->data.get_step(); i++) {
//         this->data.add_to_input_vector(this->data.get_input_vector()[i]);
//     }
// }

void SMTPath::ConnectPath(SMTPath* otherPath){
	// add the data_step into data
    this->data.set_clk(this->data.get_step() + this->data.get_clk());
	// connect the data_step of otherPath); to the data of this
	for(uint i = 0; i < otherPath->data.get_step(); i++){
		this->data.add_to_input_vector(otherPath->data.get_input_vector()[i]);
	}
}

void SMTPath::ClearPath(){
	this->data.clear_input_vector();
	this->data.set_clk(0);
}



//----------------------------SMT State-----------------------------------------
std::map<std::string, StateValue> SMTState::stateMap;
// std::vector<std::pair<std::string, int>> SMTState::get_state_at_clock(uint clock){
//     std::vector<std::pair<std::string, int>> result;
//     for (const auto& entry : stateMap) {
//         const StateValue& state = entry.second;
//         // 在 state.stateClock 中查找是否存在等于 clock 的元素
//         auto clock_iterator = std::find(state.stateClock.begin(), state.stateClock.end(), clock);
//         // 如果找到匹配的 clock 值
//         if (clock_iterator != state.stateClock.end()) {
//             // 获取相应的索引
//             size_t index = std::distance(state.stateClock.begin(), clock_iterator);
//             // 使用索引获取对应的 stateValue
//             int value = state.stateValue[index];
//             // 将结果添加到返回向量中
//             result.emplace_back(state.stateName, value);
//         }
//     }

//     return result;
// }

void SMTState::add_state(const std::string& regName, int value, uint clock) {
    auto& stateData = stateMap[regName].stateData;

    // Find if there is an existing entry with the same clock
    auto it = std::find_if(stateData.begin(), stateData.end(),
                           [clock](const StateData& pair) { return pair.clock == clock; });

    if (it != stateData.end()) {
        // If found, update the value for this clock
        it->value = value;
    } else {
        // If not found, add a new entry
        stateData.emplace_back(value, clock);
    }
}

void SMTState::clear_states() {
	stateMap.clear();
}

void SMTState::print_state(const char* file) {

	ofstream mem(file);
	for (const auto& entry : stateMap) {
		const std::string& stateName = entry.first;
		const std::vector<StateData>& stateData = entry.second.stateData;
		mem << stateName << ":\n";
		for (const auto& pair : stateData) {
			mem << "  " << pair.clock << ": " << pair.value << "\n";
		}
	}

    mem.close();
}

void SMTState::print_state_at_clock(const char* file, uint clock, double reward, unsigned int actionIndex) {
	ofstream mem(file);
	mem << "reward: "<< reward << "\n";
	mem << "Action: "<< actionIndex << "\n";
	for (const auto& entry : stateMap) {
		const std::string& stateName = entry.first;
		const std::vector<StateData>& stateData = entry.second.stateData;
		mem << stateName ;
		for (const auto& pair : stateData) {
			if (pair.clock == clock) {
				mem << ": " << pair.value << "\n";
				// mem << "  " << pair.clock << ": " << pair.value << "\n";
			}
		}
	}
	mem.close();
}




//----------------------------SMT Globals---------------------------------------
void SMTFreeAll(){
	SMTExpr::free_all();
    SMTSigCore::free_all();
}


