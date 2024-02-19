#include <cstdlib>
#include <cstring>
#include <vector>
#include <stack>
#include <algorithm>
#include "concolic.h"
#include "smt_lib.h"

using namespace std;

uint single_indent = 0;
static bool en_inst = false;

static stack<SMTAssign*> branch_stack;

unsigned get_indent(void) {
	if (single_indent) {
		single_indent = 0;
		return 0;
	}
	return g_ind;
}

static inline void inst_cond(){
	SMTAssign* smt_assign = branch_stack.top();
	smt_assign->instrument();
}

static inline void emit_block_begin(){
	fprintf(g_out, "%*cbegin\n", get_indent(), ' ');
	g_ind += g_ind_incr;
	single_indent = 0;
}

static inline void emit_block_end(){
	assert(g_ind >= g_ind_incr);
	g_ind -= g_ind_incr;
	if(g_ind == 4){
		g_ind += g_ind_incr;
		// Output the variables printed of this module
		emit_module_variables_display();
		g_ind -= g_ind_incr;
	}
	fprintf(g_out, "%*cend\n", g_ind, ' ');
	single_indent = 0;
}

static void emit_blocked_stmt(ivl_scope_t scope, ivl_statement_t stmt){
	if(ivl_statement_type(stmt) != IVL_ST_BLOCK){
		emit_block_begin();
		emit_stmt(scope, stmt);
		emit_block_end();
	} else {
		emit_stmt(scope, stmt);
	}
}

static void emit_blocked_stmt_inst(ivl_scope_t scope, ivl_statement_t stmt){
	if(ivl_statement_type(stmt) != IVL_ST_BLOCK){
		emit_block_begin();
		inst_cond();
		emit_stmt(scope, stmt);
		emit_block_end();
	} else {
		en_inst = true;
		emit_stmt(scope, stmt);
		en_inst = false;
	}
}

static void emit_stmt_block_body(ivl_scope_t scope, ivl_statement_t stmt) {
	unsigned idx, count = ivl_stmt_block_count(stmt);
	ivl_scope_t my_scope = ivl_stmt_block_scope(stmt);
	if (my_scope) emit_scope_variables(my_scope);
	else my_scope = scope;
	for (idx = 0; idx < count; idx += 1) {
		emit_stmt(my_scope, ivl_stmt_block_stmt(stmt, idx));
	}
}

static void emit_stmt_lval_name(ivl_scope_t scope, ivl_lval_t lval,
		ivl_signal_t sig) {
	ivl_expr_t array_idx = ivl_lval_idx(lval);
	emit_scope_call_path(scope, ivl_signal_scope(sig));
	emit_id(ivl_signal_basename(sig));
	
	if (array_idx) {
		int msb, lsb;
		assert(ivl_signal_dimensions(sig));
		fprintf(g_out, "[");
		lsb = ivl_signal_array_base(sig);
		msb = lsb + ivl_signal_array_count(sig) - 1;
		SMTSignal* tmp_sig = new SMTSignal(sig);
		emit_scaled_expr(scope, array_idx, msb, lsb, tmp_sig);
		fprintf(g_out, "]");
	}
}


static void emit_stmt_lval_name(ivl_scope_t scope, ivl_lval_t lval,
		ivl_signal_t sig, SMTArray **array) {
	ivl_expr_t array_idx = ivl_lval_idx(lval);
	emit_scope_call_path(scope, ivl_signal_scope(sig));
	emit_id(ivl_signal_basename(sig));
	
	if (array_idx) {
		//error("Currently arrays are not supported");
		int msb, lsb;
		assert(ivl_signal_dimensions(sig));
		fprintf(g_out, "[");
		// For an array the LSB/MSB order is not important. They are
		// always accessed from base counting up.
		lsb = ivl_signal_array_base(sig);
		msb = lsb + ivl_signal_array_count(sig) - 1;
		//SMTSignal* tmp_sig = new SMTSignal(sig);
		(*array)->parent = SMTSigCore::get_parent(sig);
		emit_scaled_expr(scope, array_idx, msb, lsb, array);
		fprintf(g_out, "]");
	}
}

static SMTSignal* emit_stmt_lval_piece(ivl_scope_t scope, ivl_lval_t lval) {
	ivl_signal_t sig = ivl_lval_sig(lval);
	ivl_expr_t sel_expr;
	ivl_select_type_t sel_type;
	unsigned width = ivl_lval_width(lval);
	int msb, lsb;
	assert(width > 0);

	/* A class supports a nested L-value so it may not have a signal
	 * at this level. */
	if (!sig) {
		error("class type lval not supported");
		//(void) emit_stmt_lval_class(scope, lval);
		return NULL;
	}

	switch (ivl_signal_data_type(sig)) {
		case IVL_VT_DARRAY:
			//emit_stmt_lval_darray(scope, lval, sig);
			//return;
		case IVL_VT_CLASS:
			error("Dynamic array or class type signal not supported");
			//(void) emit_stmt_lval_class(scope, lval);
			return NULL;
		default:
			break;
	}
	
	SMTSignal* tmp_sig = new SMTSignal(sig);
    get_sig_msb_lsb(sig, &msb, &lsb);
	/* If there are no selects then just print the name. */
	sel_expr = ivl_lval_part_off(lval);
	if (!sel_expr && (width == ivl_signal_width(sig))) {
		emit_stmt_lval_name(scope, lval, sig);
		return tmp_sig;
	}

	/* We have some kind of select. */
	sel_type = ivl_lval_sel_type(lval);
	assert(sel_expr);
	/* A bit select. */
	if (width == 1) {
		emit_stmt_lval_name(scope, lval, sig);
		fprintf(g_out, "[");
		emit_scaled_expr(scope, sel_expr, msb, lsb, tmp_sig);
		fprintf(g_out, "]");
	} else if (ivl_expr_type(sel_expr) == IVL_EX_NUMBER) {
		/* A constant part select. */
		emit_stmt_lval_name(scope, lval, sig);
		emit_scaled_range(scope, sel_expr, width, msb, lsb, tmp_sig);
	} else if (sel_type == IVL_SEL_OTHER) {
		error("Unsupported expr sel type IVL_SEL_OTHER");
	} else {
		error("Indexed part select not supported");
	}
	return tmp_sig;
}

static SMTSignal* emit_stmt_lval_piece(ivl_scope_t scope, ivl_lval_t lval, SMTArray **array) {
	ivl_signal_t sig = ivl_lval_sig(lval);
	ivl_expr_t sel_expr;
	ivl_select_type_t sel_type;
	unsigned width = ivl_lval_width(lval);
	int msb, lsb;
	assert(width > 0);

	/* A class supports a nested L-value so it may not have a signal
	 * at this level. */
	if (!sig) {
		error("class type lval not supported");
		//(void) emit_stmt_lval_class(scope, lval);
		return NULL;
	}

	switch (ivl_signal_data_type(sig)) {
		case IVL_VT_DARRAY:
			//emit_stmt_lval_darray(scope, lval, sig);
			//return;
		case IVL_VT_CLASS:
			error("Dynamic array or class type signal not supported");
			//(void) emit_stmt_lval_class(scope, lval);
			return NULL;
		default:
			break;
	}
	
	SMTSignal* tmp_sig = new SMTSignal(sig);
    get_sig_msb_lsb(sig, &msb, &lsb);
	/* If there are no selects then just print the name. */
	sel_expr = ivl_lval_part_off(lval);
	if (!sel_expr && (width == ivl_signal_width(sig))) {
		emit_stmt_lval_name(scope, lval, sig, array);
		ivl_expr_t array_idx = ivl_lval_idx(lval);
		if(array_idx){
			tmp_sig->is_array = true;
		}
		return tmp_sig;
	}

	/* We have some kind of select. */
	sel_type = ivl_lval_sel_type(lval);
	assert(sel_expr);
	/* A bit select. */
	if (width == 1) {
		emit_stmt_lval_name(scope, lval, sig);
		fprintf(g_out, "[");
		emit_scaled_expr(scope, sel_expr, msb, lsb, tmp_sig);
		fprintf(g_out, "]");
	} else if (ivl_expr_type(sel_expr) == IVL_EX_NUMBER) {
		/* A constant part select. */
		emit_stmt_lval_name(scope, lval, sig);
		emit_scaled_range(scope, sel_expr, width, msb, lsb, tmp_sig);
	} else if (sel_type == IVL_SEL_OTHER) {
		error("Unsupported expr sel type IVL_SEL_OTHER");
	} else {
		error("Indexed part select not supported");
	}
	return tmp_sig;
}

static SMTSignal* emit_stmt_lval(ivl_scope_t scope, ivl_statement_t stmt) {
	SMTSignal* sig = NULL;
	unsigned count = ivl_stmt_lvals(stmt);
	if(count != 1){
		error("Multiple lval (%s:%u)", ivl_stmt_file(stmt), ivl_stmt_lineno(stmt));
	}
	
	if (count > 1) {
		error("TODO: split into multiple assignments");
	} else {
		ivl_lval_t lval = ivl_stmt_lval(stmt, 0);
		sig = emit_stmt_lval_piece(scope, lval);
	}
	return sig;
}


static SMTSignal* emit_stmt_lval(ivl_scope_t scope, ivl_statement_t stmt, SMTArray **array) {
	SMTSignal* sig = NULL;
	unsigned count = ivl_stmt_lvals(stmt);
	if(count != 1){
		error("Multiple lval (%s:%u)", ivl_stmt_file(stmt), ivl_stmt_lineno(stmt));
	}
	
	if (count > 1) {
		error("TODO: split into multiple assignments");
		/*ivl_lval_t lval;
		fprintf(g_out, "{");
		for (uint idx = count - 1; idx > 0; idx -= 1) {
			lval = ivl_stmt_lval(stmt, idx);
			wid += ivl_lval_width(lval);
			smt_assign->lval->add(emit_stmt_lval_piece(scope, lval));
			fprintf(g_out, ", ");
		}
		lval = ivl_stmt_lval(stmt, 0);
		wid += ivl_lval_width(lval);
		smt_assign->lval->add(emit_stmt_lval_piece(scope, lval));
		fprintf(g_out, "}");*/
	} else {
		ivl_lval_t lval = ivl_stmt_lval(stmt, 0);
		sig = emit_stmt_lval_piece(scope, lval, array);
	}
	return sig;
}


/*
 * A common routine to emit the basic assignment construct. It can also
 * translate an assignment with an opcode when allowed.
 */
static SMTBlockingAssign* emit_assign_and_opt_opcode(ivl_scope_t scope, ivl_statement_t stmt) {
	fprintf(g_out, "%*c", get_indent(), ' ');
    char opcode;
	const char *opcode_str;
	SMTArray *array = new SMTArray();
	SMTExpr* lval = emit_stmt_lval(scope, stmt, &array);
	if(lval->is_array){
		lval = array;
	}
	
	/* Get the opcode and the string version of the opcode. */
	opcode = ivl_stmt_opcode(stmt);
	switch (opcode) {
		case 0: opcode_str = "";
			break;
		case 'p': opcode_str = "**";
			break;
		case '+': opcode_str = "+";
			break;
		case '-': opcode_str = "-";
			break;
		case '*': opcode_str = "*";
			break;
		case '/': opcode_str = "/";
			break;
		case '%': opcode_str = "%";
			break;
		case '&': opcode_str = "&";
			break;
		case '|': opcode_str = "|";
			break;
		case '^': opcode_str = "^";
			break;
		case 'l': opcode_str = "<<";
			break;
		case 'r': opcode_str = ">>";
			break;
		case 'R': opcode_str = ">>>";
			break;
		default:
			fprintf(stderr, "%s:%u: vlog95 error: unknown assignment operator "
					"(%c).\n",
					ivl_stmt_file(stmt), ivl_stmt_lineno(stmt),
					opcode);
			g_errors += 1;
			opcode_str = "<unknown>";
			break;
	}
	fprintf(g_out, " = ");
	SMTExpr* rval;
	if (opcode) {
		SMTBinary* smt_bi = new SMTBinary();
		smt_bi->set_opcode(opcode);
		smt_bi->add(emit_stmt_lval(scope, stmt));
		fprintf(g_out, " %s ", opcode_str);
		smt_bi->add(emit_expr(scope, ivl_stmt_rval(stmt)));
		rval = smt_bi;
	}
	else{
		if(lval->type==SMT_EXPR_SIGNAL&&ivl_expr_type(ivl_stmt_rval(stmt))==IVL_EX_NUMBER){
			int oper1_signed = ivl_signal_signed(ivl_lval_sig(ivl_stmt_lval(stmt,0)));
			ivl_expr_t oper2 = ivl_stmt_rval(stmt);
			SMTSignal* tmp_sig = dynamic_cast<SMTSignal*>(lval);
			if((int(strlen(ivl_expr_bits(oper2)))-tmp_sig->parent->width)>0)
					rval = emit_number(
						_left(ivl_expr_bits(oper2),tmp_sig->parent->width),
						tmp_sig->parent->width, 
						oper1_signed);
				else if((int(strlen(ivl_expr_bits(oper2)))-tmp_sig->parent->width)==0)
					rval = emit_number(
						ivl_expr_bits(oper2),
						tmp_sig->parent->width, 
						oper1_signed);
				else
					rval = emit_expr(scope, ivl_stmt_rval(stmt));
		}else{
			rval = emit_expr(scope, ivl_stmt_rval(stmt));
		}
	}
	fprintf(g_out, ";");
	return new SMTBlockingAssign(lval, rval);
}

/*
 * Structure to hold the port information as we extract it from the block.
 */
typedef struct port_expr_s {
	ivl_signal_port_t type;

	union {
		ivl_statement_t lval;
		ivl_expr_t rval;
	} expr;
} *port_expr_t;

static SMTBlockingAssign* emit_stmt_assign(ivl_scope_t scope, ivl_statement_t stmt) {
	return emit_assign_and_opt_opcode(scope, stmt);
}

static SMTNonBlockingAssign* emit_stmt_assign_nb(ivl_scope_t scope, ivl_statement_t stmt) {
	fprintf(g_out, "%*c", get_indent(), ' ');
	SMTArray *array = new SMTArray();
	SMTExpr* lval = emit_stmt_lval(scope, stmt, &array);
	//Transfer lval(SMTSignal) to lval(SMTArray) if lval is array
	if(lval->is_array){
		lval = array;
	}
	fprintf(g_out, " <= #1 ");
	SMTExpr* rval;
	if(lval->type==SMT_EXPR_SIGNAL&&ivl_expr_type(ivl_stmt_rval(stmt))==IVL_EX_NUMBER){
			int oper1_signed = ivl_signal_signed(ivl_lval_sig(ivl_stmt_lval(stmt,0)));
			ivl_expr_t oper2 = ivl_stmt_rval(stmt);
			SMTSignal* tmp_sig = dynamic_cast<SMTSignal*>(lval);
			if((int(strlen(ivl_expr_bits(oper2)))-tmp_sig->parent->width)>0)
					rval = emit_number(
						_left(ivl_expr_bits(oper2),(int(strlen(ivl_expr_bits(oper2)))-tmp_sig->parent->width)),
						tmp_sig->parent->width, 
						oper1_signed);
				else if((int(strlen(ivl_expr_bits(oper2)))-tmp_sig->parent->width)==0)
					rval = emit_number(
						ivl_expr_bits(oper2),
						tmp_sig->parent->width, 
						oper1_signed);
				else
					rval = emit_expr(scope, ivl_stmt_rval(stmt));
		}else{
			rval = emit_expr(scope, ivl_stmt_rval(stmt));
		}
	// SMTExpr* rval = emit_expr(scope, ivl_stmt_rval(stmt));
	fprintf(g_out, ";");
	return new SMTNonBlockingAssign(lval, rval);
}

static void emit_stmt_block(ivl_scope_t scope, ivl_statement_t stmt) {
	emit_block_begin();
	if(en_inst) {
		inst_cond();
	}
	emit_stmt_block_body(scope, stmt);
	emit_block_end();
}

static void emit_stmt_block_named(ivl_scope_t scope, ivl_statement_t stmt) {
	ivl_scope_t my_scope = ivl_stmt_block_scope(stmt);
	fprintf(g_out, "%*cbegin: ", get_indent(), ' ');
	emit_id(ivl_scope_basename(my_scope));
	fprintf(g_out, "\n");
	emit_stmt_block_body(scope, stmt);
	fprintf(g_out, "%*cend  /* %s */\n", get_indent(), ' ',
			ivl_scope_basename(my_scope));
}

static void emit_stmt_case(ivl_scope_t scope, ivl_statement_t stmt) {
	const char *case_type = 0;
	unsigned idx, default_case, count = ivl_stmt_case_count(stmt);
	switch (ivl_statement_type(stmt)) {
		case IVL_ST_CASE:
		case IVL_ST_CASER:
			case_type = "case";
			break;
		case IVL_ST_CASEX:
			case_type = "casex";
			break;
		case IVL_ST_CASEZ:
			case_type = "casez";
			break;
		default:
			error("Unknown case type");
	}
	SMTBranchNode* parent_node = new SMTBranchNode();
	
	fprintf(g_out, "%*c%s (", get_indent(), ' ', case_type);
	ivl_expr_t cond_expr = ivl_stmt_cond_expr(stmt);
	parent_node->cond = emit_expr(scope, cond_expr);
	parent_node->cond_width = ivl_expr_width(cond_expr);
	fprintf(g_out, ")\n");
	g_ind += g_ind_incr;
	default_case = count;
    SMTBasicBlock* top_bb = SMTProcess::curr_proc->top_bb;
	//top_bb->weight = 1;
    SMTBasicBlock* exit_bb = new SMTBasicBlock();
	exit_bb->idom = top_bb;
	for (idx = 0; idx < count; idx += 1) {
		ivl_expr_t expr = ivl_stmt_case_expr(stmt, idx);
		/* This is the default case so emit it last. */
		if (expr == 0) {
			assert(default_case == count);
			default_case = idx;
			continue;
		}
		fprintf(g_out, "%*c", get_indent(), ' ');
		SMTExpr* case_expr = emit_expr(scope, expr);
		SMTBranch* smt_br = SMTBranch::create_case_branch(parent_node, case_expr);
        SMTBasicBlock* smt_bb = new SMTBasicBlock();
		smt_br->block = smt_bb;
		smt_bb->weight = 1;
        smt_bb->assign_list.push_back(smt_br);
        smt_bb->predecessors.insert(top_bb);
		smt_bb->idom = top_bb;
        top_bb->successors.insert(smt_bb);
        
		fprintf(g_out, " :\n");
		g_ind += g_ind_incr;
		single_indent = 0;
		branch_stack.push(smt_br);
        SMTProcess::curr_proc->top_bb = smt_bb;
		emit_blocked_stmt_inst(scope, ivl_stmt_case_stmt(stmt, idx));
		branch_stack.pop();
		g_ind -= g_ind_incr;
        exit_bb->predecessors.insert(SMTProcess::curr_proc->top_bb);
        SMTProcess::curr_proc->top_bb->successors.insert(exit_bb);
	}
	if (default_case < count) {
		fprintf(g_out, "%*cdefault:\n", get_indent(), ' ');
		g_ind += g_ind_incr;
		single_indent = 0;
		SMTBranch* smt_br = SMTBranch::create_default_branch(parent_node);
        SMTBasicBlock* smt_bb = new SMTBasicBlock();
		smt_br->block = smt_bb;
        smt_bb->assign_list.push_back(smt_br);
        smt_bb->predecessors.insert(top_bb);
		smt_bb->idom = top_bb;
        top_bb->successors.insert(smt_bb);
        
		branch_stack.push(smt_br);
        SMTProcess::curr_proc->top_bb = smt_bb;
		emit_blocked_stmt_inst(scope, ivl_stmt_case_stmt(stmt, default_case));
		branch_stack.pop();
		g_ind -= g_ind_incr;
        exit_bb->predecessors.insert(SMTProcess::curr_proc->top_bb);
        SMTProcess::curr_proc->top_bb->successors.insert(exit_bb);
	}
	assert(g_ind >= g_ind_incr);
	g_ind -= g_ind_incr;
	fprintf(g_out, "%*cendcase\n", get_indent(), ' ');
    
    SMTProcess::curr_proc->top_bb = exit_bb;
}

static SMTBlockingAssign* emit_stmt_cassign(ivl_scope_t scope, ivl_statement_t stmt) {
	fprintf(g_out, "%*cassign ", get_indent(), ' ');
	SMTExpr* lval = emit_stmt_lval(scope, stmt);
	fprintf(g_out, " = ");
	SMTExpr* rval = emit_expr(scope, ivl_stmt_rval(stmt));
	fprintf(g_out, ";");
	return new SMTBlockingAssign(lval, rval);
}

static void emit_stmt_condit(ivl_scope_t scope, ivl_statement_t stmt) {
    //SMTBranchCondit* branch_node = new SMTBranchCondit();
	SMTBranchNode* branch_node = new SMTBranchNode();
	ivl_statement_t true_stmt = ivl_stmt_cond_true(stmt);
	ivl_statement_t false_stmt = ivl_stmt_cond_false(stmt);
	ivl_expr_t cond_expr = ivl_stmt_cond_expr(stmt);
	fprintf(g_out, "%*cif (", get_indent(), ' ');
	branch_node->cond = emit_expr(scope, cond_expr);
    branch_node->cond_width = ivl_expr_width(cond_expr);
	fprintf(g_out, ")");
    SMTBranch* true_br = SMTBranch::create_true_branch(branch_node);
    SMTBranch* false_br = SMTBranch::create_false_branch(branch_node);
    
    SMTBasicBlock* true_bb = new SMTBasicBlock();
    SMTBasicBlock* false_bb = new SMTBasicBlock();
	true_bb->weight = 1;
	false_bb->weight = 1;
	//SMTProcess::curr_proc->top_bb->weight = 1;
	true_br->block = true_bb;
	false_br->block = false_bb;
	true_bb->idom = SMTProcess::curr_proc->top_bb;
	false_bb->idom = SMTProcess::curr_proc->top_bb;
    true_bb->predecessors.insert(SMTProcess::curr_proc->top_bb);
    false_bb->predecessors.insert(SMTProcess::curr_proc->top_bb);
    SMTProcess::curr_proc->top_bb->successors.insert(true_bb);
    SMTProcess::curr_proc->top_bb->successors.insert(false_bb);
    true_bb->assign_list.push_back(true_br);
    false_bb->assign_list.push_back(false_br);
    SMTBasicBlock* exit_bb = new SMTBasicBlock();
    exit_bb->idom = SMTProcess::curr_proc->top_bb;;
	
    SMTProcess::curr_proc->top_bb = true_bb;
    single_indent = 1;
	if (true_stmt) {
		branch_stack.push(true_br);
		emit_blocked_stmt_inst(scope, true_stmt);
		branch_stack.pop();
	} else {
        emit_block_begin();
        true_br->instrument();
        emit_block_end();
	}
    exit_bb->predecessors.insert(SMTProcess::curr_proc->top_bb);
    SMTProcess::curr_proc->top_bb->successors.insert(exit_bb);
    
    
    fprintf(g_out, "%*c", get_indent(), ' ');
	fprintf(g_out, "else");
    SMTProcess::curr_proc->top_bb = false_bb;
    single_indent = 1;
	if (false_stmt) {
		branch_stack.push(false_br);
		emit_blocked_stmt_inst(scope, false_stmt);
		branch_stack.pop();
	} else{
        emit_block_begin();
        false_br->instrument();
        emit_block_end();
    }
    exit_bb->predecessors.insert(SMTProcess::curr_proc->top_bb);
    SMTProcess::curr_proc->top_bb->successors.insert(exit_bb);
    
    SMTProcess::curr_proc->top_bb = exit_bb;
}

/*
 * Currently a do/while can be translated in two ways: no named blocks in
 * the do/while statement and only a named block as the top level statement.
 * Anything else cannot be translated since a hierarchical variable reference
 * or disable cannot work correctly when the statement is duplicated.
 */
typedef enum stmt_named_type_e {
	NO_NAMED = 0,
	TOP_NAMED = 1,
	OTHER_NAMED = 2
} stmt_named_type_t;

static stmt_named_type_t get_named_type(ivl_statement_t stmt, unsigned is_top);

/*
 * A block can start as either a NO_NAMED or a TOP_NAMED so pass this in.
 */
static stmt_named_type_t get_named_type_block(ivl_statement_t stmt,
		stmt_named_type_t def_type) {
	stmt_named_type_t rtn = def_type;
	unsigned idx, count = ivl_stmt_block_count(stmt);
	for (idx = 0; idx < count; idx += 1) {
		stmt_named_type_t lrtn;
		lrtn = get_named_type(ivl_stmt_block_stmt(stmt, idx), 0);
		if (lrtn > rtn) rtn = lrtn;
		if (rtn == OTHER_NAMED) break;
	}
	return rtn;
}

static stmt_named_type_t get_named_type_case(ivl_statement_t stmt) {
	stmt_named_type_t rtn = NO_NAMED;
	unsigned idx, count = ivl_stmt_case_count(stmt);
	for (idx = 0; idx < count; idx += 1) {
		stmt_named_type_t lrtn;
		lrtn = get_named_type(ivl_stmt_case_stmt(stmt, idx), 0);
		if (lrtn > rtn) rtn = lrtn;
		if (rtn == OTHER_NAMED) break;
	}
	return rtn;
}

static stmt_named_type_t get_named_type_condit(ivl_statement_t stmt) {
	stmt_named_type_t true_rtn, false_rtn;
	true_rtn = get_named_type(ivl_stmt_cond_true(stmt), 0);
	false_rtn = get_named_type(ivl_stmt_cond_false(stmt), 0);
	if (false_rtn > true_rtn) return false_rtn;
	return true_rtn;
}

static stmt_named_type_t get_named_type(ivl_statement_t stmt, unsigned is_top) {
	stmt_named_type_t rtn = NO_NAMED;

	if (!stmt) return NO_NAMED;

	switch (ivl_statement_type(stmt)) {
		case IVL_ST_BLOCK:
		case IVL_ST_FORK:
		case IVL_ST_FORK_JOIN_ANY:
		case IVL_ST_FORK_JOIN_NONE:
			/* Check to see if this is a named top block or sub block. */
			if (ivl_stmt_block_scope(stmt)) {
				if (is_top) rtn = TOP_NAMED;
				else return OTHER_NAMED;
			}
			rtn = get_named_type_block(stmt, rtn);
			break;
		case IVL_ST_CASE:
		case IVL_ST_CASER:
		case IVL_ST_CASEX:
		case IVL_ST_CASEZ:
			rtn = get_named_type_case(stmt);
			break;
		case IVL_ST_CONDIT:
			rtn = get_named_type_condit(stmt);
			break;
			/* These statements only have a single sub-statement that is not
			 * a top level statement relative to the original do/while. */
		case IVL_ST_DELAY:
		case IVL_ST_DELAYX:
		case IVL_ST_DO_WHILE:
		case IVL_ST_FOREVER:
		case IVL_ST_REPEAT:
		case IVL_ST_WAIT:
		case IVL_ST_WHILE:
			rtn = get_named_type(ivl_stmt_sub_stmt(stmt), 0);
			break;
		default:
			/* The rest of the statement types do not have sub-statements. */
			break;
	}
	return rtn;
}

static void emit_stmt_wait(ivl_scope_t scope, ivl_statement_t stmt) {
	fprintf(g_out, " @(");
	emit_event(scope, stmt);
	fprintf(g_out, ")");
	single_indent = 1;
	emit_blocked_stmt(scope, ivl_stmt_sub_stmt(stmt));
}

static void emit_stmt_stask(ivl_scope_t scope, ivl_statement_t stmt) {
	const char* name = ivl_stmt_name(stmt);
	if(strcmp(name, "$display")){
		error("Only $display system call is supported. Used \"%s\" at %s:%u", name, ivl_stmt_file(stmt), ivl_stmt_lineno(stmt));
	}
	
	fprintf(g_out, "%*c%s(", get_indent(), ' ', name);
	const uint count = ivl_stmt_parm_count(stmt);
	if(count){
		emit_expr(scope, ivl_stmt_parm(stmt, 0));
	}
	for(uint i = 1; i < count; i++){
		fprintf(g_out, ", ");
		emit_expr(scope, ivl_stmt_parm(stmt, i));
	}
	fprintf(g_out, ");\n");
	
	//SMTBasicBlock::add_target(SMTProcess::curr_proc->top_bb);
}

void emit_stmt(ivl_scope_t scope, ivl_statement_t stmt) {
	switch (ivl_statement_type(stmt)) {
		case IVL_ST_ASSIGN:
			emit_stmt_assign(scope, stmt);
			break;
		case IVL_ST_ASSIGN_NB:
			emit_stmt_assign_nb(scope, stmt);
			break;
		case IVL_ST_BLOCK:
			if (ivl_stmt_block_scope(stmt)) {
				error("TODO: Named block");
				emit_stmt_block_named(scope, stmt);
			} else {
				emit_stmt_block(scope, stmt);
			}
			break;
		case IVL_ST_CASE:
		case IVL_ST_CASER:
		case IVL_ST_CASEX:
		case IVL_ST_CASEZ:
			emit_stmt_case(scope, stmt);
			break;
		case IVL_ST_CASSIGN:
			error("CASSIGN entry (%s:%u)", ivl_stmt_file(stmt), ivl_stmt_lineno(stmt));
			emit_stmt_cassign(scope, stmt);
			break;
		case IVL_ST_CONDIT:
			emit_stmt_condit(scope, stmt);
			break;
		case IVL_ST_STASK:
			//System task. Emit as is
			emit_stmt_stask(scope, stmt);
			break;
		case IVL_ST_DELAY:
			//emit_stmt_delay(scope, stmt);
			//break;
		case IVL_ST_DELAYX:
			//emit_stmt_delayx(scope, stmt);
			break;
		case IVL_ST_WAIT:
			emit_stmt_wait(scope, stmt);
			break;
		case IVL_ST_NOOP:
			// If this is a statement termination then just finish the
			// statement, otherwise print an empty begin/end pair.
            //error("check: IVL_ST_NOOP is entered");
			if (single_indent) {
				single_indent = 0;
				fprintf(g_out, ";\n");
			} else {
				fprintf(g_out, "%*cbegin\n", get_indent(), ' ');
				fprintf(g_out, "%*cend\n", get_indent(), ' ');
			}
			break;
		default:
			error("Unsupported statement type: %u (%s:%u)", ivl_statement_type(stmt), ivl_stmt_file(stmt), ivl_stmt_lineno(stmt));
			break;
	}
	single_indent = 0;
}

SMTProcess* emit_process(ivl_scope_t scope, ivl_process_t proc) {
    SMTProcess* smt_proc = new SMTProcess();
	ivl_statement_t stmt = ivl_process_stmt(proc);
	if (ivl_statement_type(stmt) == IVL_ST_NOOP) {
		info("Process statement type IVL_ST_NOOP (%s,%u)", ivl_process_file(proc), ivl_process_lineno(proc));
		return smt_proc;
	}
	//reset indent
	g_ind = g_ind_incr;
	
	fprintf(g_out, "%*c", g_ind, ' ');
	fprintf(g_out, "// Following code segment is generated from %s:%u\n", ivl_process_file(proc), ivl_process_lineno(proc));
	fprintf(g_out, "%*c", g_ind, ' ');
	switch (ivl_process_type(proc)) {
		case IVL_PR_INITIAL:
			fprintf(g_out, "initial");
			break;
		case IVL_PR_ALWAYS:
			fprintf(g_out, "always");
			break;
		default:
			error("Unsupported process type (%s:%u)", ivl_process_file(proc), ivl_process_lineno(proc));
	}

	single_indent = 1;
	if(ivl_statement_type(stmt) != IVL_ST_WAIT){
        error("process without sensitivity list");
		emit_blocked_stmt(scope, stmt);
	} else {
		SMTProcess::curr_proc = smt_proc;
		emit_stmt(scope, stmt);
        smt_proc->exit_block = smt_proc->top_bb;
		SMTProcess::curr_proc = NULL;
	}
	fprintf(g_out, "\n");
    return smt_proc;
}
