#pragma once

#include "ivl_target.h"
#include "concolic.h"
#include "data_mem.h"
#include <vector>
#include <stack>
#include <string>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <set>
#include <list>

//Forward declarations
class SMTAssign;
class SMTSignal;
class SMTExpr;
class SMTConcat;
class SMTCust;
class SMTBinary;
class SMTUnary;
class SMTTernary;
class SMTNumber;
class SMTBranchNode;
class SMTBranch;
class SMTSigCore;
class SMTBasicBlock;
class SMTProcess;
class SMTPath;
struct constraint_t;

typedef struct{
	constraint_t* cnst;
	SMTBranch* br;
}br_cnst_t;

//----------------------------SMT Commons---------------------------------------
typedef enum{
	SMT_CLK_CURR,
	SMT_CLK_NEXT
}SMTClkType;

typedef enum{
	SMT_ASSIGN_CONT,
	SMT_ASSIGN_BLOCKING,
	SMT_ASSIGN_NON_BLOCKING,
	SMT_ASSIGN_BRANCH
}SMTAssignType;

typedef enum{
    SMT_BRANCH_CONDIT,
    SMT_BRANCH_CASE
}SMTBranchType;

typedef enum{
	SMT_EXPR_UNSPECIFIED,	// Unspecified leaf node.

	SMT_EXPR_CONCAT,		// Concatenation of nodes
	SMT_EXPR_UNARY,			// Unary node
	SMT_EXPR_BINARY,		// Binary node
	SMT_EXPR_LOGIC,			// Logic gate node
    SMT_EXPR_CUSTOM,        // Custom node
	SMT_EXPR_TERNARY,		// Ternary node
	SMT_EXPR_NUMBER,		// Number node, for constants. Leaf node.
	SMT_EXPR_SIGNAL,		// Signal node, for nets. Leaf Node.
	SMT_EXPR_STRING,			// String, mostly from display type statements
	SMT_EXPR_ARRAY
}SMTExprType;

typedef enum{
	CNST_CLK,
	CNST_ASSIGN,
	CNST_ASSIGN_NON_BLOCKING,
	CNST_BRANCH
}constraint_type_t;

struct constraint_t;

typedef struct constraint_t{
	constraint_type_t type;
	uint clock;
	SMTAssign* obj;
	term_t yices_term;
	uint index;
    uint hash_value;
}constraint_t;

//----------------------------SMT Expr------------------------------------------
class SMTExpr{
private:
	static std::vector<SMTExpr*> all_expr_list;
	
protected:	
	SMTExpr(const SMTExprType _type);
		
public:
	const SMTExprType type;
	bool is_bool;		//bool or bitvector
	bool is_inverted;	//inverted if true
	std::vector<SMTExpr*> expr_list;
	term_t yices_term;
	bool is_term_eval_needed;
	
	virtual ~SMTExpr();
	virtual void add(SMTExpr *expr);
    virtual SMTExpr* get_child(uint idx);
	virtual SMTExpr* get_expanded() = 0;
	virtual term_t eval_term(SMTClkType clk) = 0;
	virtual void print(std::stringstream &ss) = 0;
	
	static void free_all();
};

//------------------------------SMT String--------------------------------------
class SMTString: public SMTExpr{
public:
	SMTString(const char* str);
	
	void print(std::stringstream& ss) override;
	term_t eval_term(SMTClkType clk) override;
	SMTExpr* get_expanded() override;
	
private:
	const char* _string;
};

//----------------------------SMT Unspecified-----------------------------------
class SMTUnspecified: public SMTExpr{
public:
	SMTUnspecified();
	
	void print(std::stringstream& ss) override;
	term_t eval_term(SMTClkType clk) override;
	SMTExpr* get_expanded() override;
};


//----------------------------SMT Assign----------------------------------------
class SMTAssign{
private:
	static std::vector<SMTAssign*> assign_map;
	bool is_first_print;
	bool is_first_print_exp;
	
protected:
    const SMTClkType clk_type;
	bool covered_any_clock;
    uint covered_in_sim;
	std::stringstream ss;
	std::stringstream ss_exp;
	SMTExpr* expanded_rval;
	SMTExpr* expanded_lval;
	term_t expanded_term;
	term_t unexpanded_term;
	
	SMTAssign(SMTClkType clk_type, SMTAssignType assign_type, 
				SMTExpr* lval, SMTExpr* rval, bool is_commit);
    virtual void init_assign();
    
public:
    const SMTAssignType assign_type;
    const uint id;
	SMTExpr* lval;
	SMTExpr* rval;
    const bool is_commit;
	term_t yices_term;
	SMTBasicBlock* block;
	SMTProcess* process;
	
    virtual void instrument();
	virtual term_t update_term();
	virtual term_t get_current_term();
	virtual term_t get_expanded_term();
	virtual term_t update_expanded_term();
	virtual bool is_covered();
	virtual void set_covered(uint sim_num);
    virtual void partial_assign_check();
    virtual SMTSigCore* get_lval_sig_core();
	virtual std::string print();
	virtual std::string print_expanded();
	virtual SMTExpr* get_expanded_rval();
	virtual SMTExpr* get_expanded_lval();
	virtual void add_to_cont();
	
	static SMTAssign* get_assign(uint id);
	static uint get_assign_count();
	static void print_coverage(std::ofstream &report);
};


//------------------------SMT Blocking Assign-----------------------------------
class SMTBlockingAssign: public SMTAssign{
public:
	SMTBlockingAssign(SMTExpr* lval, SMTExpr* rval);
};


//----------------------SMT Non Blocking Assign---------------------------------
class SMTNonBlockingAssign: public SMTAssign{
public:
	SMTNonBlockingAssign(SMTExpr* lval, SMTExpr* rval);
};


//-----------------------------SMT Branch---------------------------------------
class SMTBranch: public SMTAssign{
private:
	static std::vector<SMTBranch*> all_branches_list;
	bool* coverage;			//coverage at index clock
	
	SMTBranch(SMTBranchNode* parent_node, SMTBranchType type, SMTExpr* lval, 
			SMTExpr* rval);
	static SMTBranch* _create_condit_branch(SMTBranchNode* parent, const char* num_expr);
	//bool check_is_dep_expr(SMTExpr* expr);
	bool saved_coverage;
    static uint saved_total_branch;
	static uint saved_covered_branch;
	
public:
	static uint total_branch_count;
	static uint covered_branch_count;
	const SMTBranchType type;
    const uint list_idx;    //index in parent's branch list
	SMTBranchNode*  parent_node;
	uint k_permit_covered;
	bool is_dep;
	
	virtual ~SMTBranch();
	
	bool is_covered_clk(uint clock);
	void set_covered_clk(uint sim_num, uint clock);
    void clear_covered_clk(uint clock);
    //void update_distance();
	void update_edge();
	term_t update_term() override;
	void instrument() override;

	
	static SMTBranch* create_true_branch(SMTBranchNode* parent);
	static SMTBranch* create_false_branch(SMTBranchNode* parent);
	static SMTBranch* create_case_branch(SMTBranchNode* parent, SMTExpr* case_expr);
	static SMTBranch* create_default_branch(SMTBranchNode* parent);
	static void clear_k_permit();
    static void clear_coverage(uint min_clock);
	static void save_coverage();
	static void restore_coverage();
	static void get_state_variables(std::set<SMTSigCore*> &sigs, SMTExpr* expr);
};


//-------------------------SMT Branch Node--------------------------------------
class SMTBranchNode{
public:
    SMTBranchNode();
    std::vector<SMTBranch*> branch_list;
    SMTExpr* cond;
    uint cond_width;
};


//----------------------------SMT Concat----------------------------------------
class SMTConcat : public SMTExpr{
public:
	uint repeat;
	SMTConcat();
	void print(std::stringstream& ss) override;
	term_t eval_term(SMTClkType clk) override;
	SMTExpr* get_expanded() override;
};

//-----------------------------SMT Array----------------------------------------
class SMTArray: public SMTExpr{
public:
	SMTSigCore* parent;
	int width; //! array width
	int index_width; //! array index width

	SMTExpr* _index;
	
	bool is_index_term;
	SMTSigCore* index_term;
	int array_index;
	std::string array_index_str;
	
	SMTArray();
	SMTExpr* get_expanded() override;
	void print(std::stringstream& ss) override;
	void update_index_term();
	term_t eval_term(SMTClkType clk) override;
	term_t update_term();
};


//-----------------------------SMT Cust-----------------------------------------
class SMTCust : public SMTExpr{
private:
    std::string operand;
    
public:
    SMTCust(std::string operand);
	void print(std::stringstream& ss) override;
	term_t eval_term(SMTClkType clk) override;
	SMTExpr* get_expanded() override;
};



//----------------------------SMT Unary----------------------------------------
class SMTUnary : public SMTExpr{
private:
    std::string opcode;
	term_t (*func)(term_t inp);
	
public:
	SMTUnary();
    //SMTUnary(SMTUnaryOpcode opcode);
    
    void set_opcode(const ivl_expr_t expr);
	void set_opcode(char ivl_code);
	void print(std::stringstream& ss) override;
	term_t eval_term(SMTClkType clk) override;
	SMTExpr* get_expanded() override;
};


//----------------------------SMT Binary----------------------------------------
class SMTBinary : public SMTExpr{
private:
	std::string opcode;
	term_t (*func)(term_t in1, term_t in2);
	
public:
	bool is_signed;
	SMTBinary();
	void print(std::stringstream& ss) override;
	void set_opcode_from_expr(const ivl_expr_t expr);
	void set_opcode(char ivl_code);
	term_t eval_term(SMTClkType clk) override;
	SMTExpr* get_expanded() override;

};


//----------------------------SMT Logic-----------------------------------------
class SMTLogic : public SMTExpr{
private:
	char opcode;
    std::string get_opcode();
	term_t (*func)(term_t in1, term_t in2);
	
public:
	SMTLogic();
	void print(std::stringstream& ss) override;
	void set_opcode(char ivl_code, bool is_inverted);
	term_t eval_term(SMTClkType clk) override;
	SMTExpr* get_expanded() override;
};


//----------------------------SMT Ternary----------------------------------------
class SMTTernary : public SMTExpr{	
public:
	SMTTernary();
	void print(std::stringstream& ss) override;
	term_t eval_term(SMTClkType clk) override;
	SMTExpr* get_expanded() override;

};


//----------------------------SMT Number----------------------------------------
class SMTNumber: public SMTExpr{
private:
	std::string value_bin;
    std::string value_hex;
	bool is_signed;
	
public:
	bool has_undef;
	uint width;
	
	SMTNumber(const char* bits, int bit_width, bool is_signed);
	void print(std::stringstream& ss) override;
	void emit_verilog_value();
	term_t eval_term(SMTClkType clk) override;
	bool is_equal(SMTNumber* num);
	SMTExpr* get_expanded() override;
};


//---------------------------SMT Signal Core------------------------------------
class SMTSigCore {
private:
	static std::unordered_map<ivl_signal_t, SMTSigCore*> sig_to_core_map;
    static std::vector<SMTSigCore*> reg_list;
    static std::vector<SMTSigCore*> input_list;		//same as input list
    char *zeros;
	std::vector<uint> version_at_clock;
	bool was_in_queue;
	term_t init_term;
    
public:
	std::vector<uint> term_stack;
	type_t bv_type;
	uint curr_version;
    uint next_version;
    std::string name;
    int width;
	int index_width;
    bool is_dep;
	bool is_state_variable;
	SMTAssign* cont_assign;
	
	std::vector<SMTAssign*> assignments;
	
    SMTSigCore(ivl_signal_t sig);
    virtual ~SMTSigCore();
    
    void commit();
    void update_next_version();     //increments and prints next version define
	term_t get_term(SMTClkType clk);
    
    static void free_all();
    static SMTSigCore* get_parent(ivl_signal_t sig);
    static void clear_all_versions();
    static void commit_versions(uint clock);
	static void restore_versions(uint clock);
	static void yices_insert_reg_init(context_t * ctx);
	static void print_state_variables(std::ofstream &out);
    static void set_input_version(uint version);
};


//----------------------------SMT Signal----------------------------------------
void add_to_rval(std::set<SMTSigCore*>& assign_set, SMTExpr* expr);
class SMTSignal: public SMTExpr{
public:
    SMTSigCore* parent;
	int lsb;
	int msb;
	
    SMTSignal();	
    SMTSignal(ivl_signal_t sig);
	SMTExpr* get_expanded() override;
    void print(std::stringstream& ss) override;
	term_t eval_term(SMTClkType clk) override;
};


//----------------------------SMT Process---------------------------------------
class SMTProcess{
public:
    SMTProcess();
	//SMTProcess(SMTProcess &obj);
    virtual ~SMTProcess();
    
    bool is_edge_triggered;
    
    SMTBasicBlock* entry_block;
    SMTBasicBlock* exit_block;
    SMTBasicBlock* top_bb;
	static SMTProcess* curr_proc;
    void add_assign(SMTAssign* assign);
    
    void expand();
    static void combine_processes();
    static void make_circular();
	
private:
    bool is_expanded;
    bool expanding_now;
    static std::vector<SMTProcess*> process_list;
    static SMTProcess g_comb_process;
};


//--------------------------SMT Basic Block-------------------------------------
class SMTBasicBlock{
public:
	SMTBasicBlock();
	virtual ~SMTBasicBlock();
    const uint id;
    std::vector<SMTAssign*> assign_list;
    std::set<SMTBasicBlock*> successors;
    std::set<SMTBasicBlock*> predecessors;
	
	SMTBasicBlock* idom;
	uint weight;
	uint distance;
	bool is_edge_updated;
    
    void print(std::ofstream &out);
    void update_distance();
	void update_distance_from_adjacency_list();
	void update_edge();
    
    static void print_all(std::ofstream &out);
	static void reset_flags();
	static void add_target(SMTBasicBlock* target);
	//static void remove_target(SMTBasicBlock* target);
    static void remove_covered_targets(uint iter);
	//static void remove_dominator_targets();
    static void edge_realignment();
    static SMTBasicBlock* get_target();
	static uint target_num();
	static void update_all_closest_paths(SMTPath* path, const std::vector<constraint_t*> &constraints_stack);
	static void update_all_distances();
	void dump_distances();
	void update_closest_path(SMTPath* path, const std::vector<constraint_t*> &constraints_stack);
	std::pair<uint, uint> distance_from_current_path(const std::vector<constraint_t*> &constraints_stack);
	static std::list<SMTBasicBlock*> target_list;
	SMTPath* closest_path;
	uint* adjacency_list;
uint closest_path_distance = 0xFFFFFFF;
uint closest_path_clock = 0xFFFFFFF;
	
private:
    void print_assigns(std::ofstream &out);
    static uint id_counter;
    static std::vector<SMTBasicBlock*> block_list;
	const static uint initial_distance = 0xFFFFFFF;
	
	
};


//--------------------------------SMT Path--------------------------------------
class SMTPath{
public:
	SMTPath(CTDataMem &curr_data);
	virtual ~SMTPath();

	CTDataMem data;
};
