#include "concolic.h"
#include "globals.h"
#include "smt_lib.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <list>
#include <cstdlib>
#include <string>
#include <fstream>
#include <set>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>

using namespace std;


// store the blocks
std::unordered_set<unsigned int> prev_ids;
std::unordered_set<unsigned int> curr_ids;

//configuration
const bool		enable_error_check = false;
const bool		enable_obs_padding = true;
const bool		enable_sim_copy = false;
const bool		enable_yices_debug = true;
const uint      iteration_limit = 100;
const uint      total_limit = 1000;
unordered_map<SMTBasicBlock*, uint> iter_count;

static inline void free_stack();
context_t *yices_context;
static vector<constraint_t*> constraints_stack;
static char sim_file_name[64];
//static char cnst_file_name[64];
static char mem_file_name[64];
static uint hash_table[1024*16];
static uint sim_num;


static SMTBranch* selected_branch;
static uint selected_clock;

static void check_satisfiability();

static inline void compile() {
    string cmd = "iverilog -o conc_run.vvp " + string(g_output_file) + \
		  " " + string(g_tb_file);
	printf("%s\n", cmd.c_str());
	system(cmd.c_str());
}

static constraint_t* create_clock(uint clock){
	constraint_t* cnst = new constraint_t;
	cnst->type = CNST_CLK;
	cnst->clock = clock;
	cnst->obj = NULL;
    cnst->hash_value = 0;
	cnst->index = 0;
	cnst->yices_term = -1;
    return cnst;
}

static set<uint> path_hash_map;
static uint g_hash_value;
static void update_path_hash_map(constraint_t* cnst){
    cnst->hash_value = g_hash_value;
    g_hash_value += hash_table[cnst->obj->id % 16384]*cnst->index;
    path_hash_map.insert(g_hash_value);
}

static void smt_yices_dump_error(){
    //reset context
    yices_reset_context(yices_context);

    //insert initial assertion to zero for registers
    SMTSigCore::yices_insert_reg_init(yices_context);

    FILE* f_dbg = fopen("debug.txt", "w");
    
    //dump yices constraints
    for(auto it:constraints_stack){
		if(it->type != CNST_CLK){
            fprintf(f_dbg, "[%3u]    ", it->obj->id);
            yices_pp_term(f_dbg, it->yices_term, 1000, 1, 0);
            yices_assert_formula(yices_context, it->yices_term);
            if(yices_check_context(yices_context, NULL) == STATUS_UNSAT){
                break;
            }
		}
	}
    
    fclose(f_dbg);
}

static constraint_t* create_constraint(uint clock, SMTAssign* assign){
	constraint_t* cnst = new constraint_t;
	cnst->clock = clock;
	cnst->obj = assign;
	cnst->index = constraints_stack.size();
    cnst->yices_term = assign->update_term();//是assign错了
    if(cnst->yices_term <= 0){
        yices_print_error(stdout);
        error("Term evaluation failed at assign id: %u", assign->id);
    }
    
	if(assign->assign_type == SMT_ASSIGN_BRANCH){
		cnst->type = CNST_BRANCH;
		SMTBranch* br = dynamic_cast<SMTBranch*>(assign);
		assert(br);
        update_path_hash_map(cnst);
    	br->set_covered_clk(sim_num, clock);
	} else{
        cnst->type = CNST_ASSIGN;
		assign->set_covered(sim_num);
    }
    return cnst;
}

static void write_first_clock(const char* file_name){
	FILE* f_test = fopen(file_name, "r");
	if(f_test == NULL){
		perror("Error opening file!");
		return;
	}
	// Get the size of file
	fseek(f_test, 0, SEEK_END);
	long f_size = ftell(f_test);
	fseek(f_test, 0, SEEK_SET);

	// Read the content to buffer
	char* buffer = (char*)malloc(f_size + 1);
	fread(buffer, f_size, 1, f_test);
	buffer[f_size] = '\0';
	fclose(f_test);

	// Write the first clock and the rest
	f_test = fopen(file_name, "w");
	if (f_test == NULL) {
    	perror("Error opening file");
    	free(buffer);
    	return;
	}
	fprintf(f_test, ";_C%11u\n", 0);
	fprintf(f_test, "%s", buffer);
	free(buffer);
	fclose(f_test);
}

static void build_stack() {
	FILE* f_test = NULL;
	if(enable_sim_copy){
		f_test = fopen(sim_file_name, "r");
		write_first_clock(sim_file_name);
	} else{
		f_test = fopen("sim.log", "r");
		write_first_clock("sim.log");
	}
	assert(f_test);
	uint clock = 0;
    g_hash_value = 0;
	char tag[16];
	uint val;
	
	is_new_block = false;

	//constraints_stack.push_back(create_clock(0));
	while(true){
		fscanf(f_test, "%s%u", tag, &val);
		if(strcmp(tag, ";_C") == 0){
			clock = val;
			if(clock == g_unroll + 1)	break;
			constraints_stack.push_back(create_clock(clock));
            SMTSigCore::set_input_version(clock);
            SMTSigCore::commit_versions(clock);
		}
		else if (strcmp(tag, ";A") == 0){
			SMTAssign* assign = SMTAssign::get_assign(val);
			constraints_stack.push_back(create_constraint(clock, assign));
			curr_ids.insert(assign->block->id);
		}
	}
	// examine if cover the new block
	if(!is_new_block){
		for(auto id:curr_ids){
			if(prev_ids.find(id) == prev_ids.end()){
				is_new_block = true;
				break;
			}
		}
	}
	prev_ids = curr_ids;
	curr_ids.clear();

	// printf("If the new block is covered: %d\n", is_new_block);
	fclose(f_test);
}

static inline void free_stack() {
	while(!constraints_stack.empty()){
		delete constraints_stack.back();
		constraints_stack.pop_back();
	}
}

void init_hash_table(){
    for(uint i=0; i< 1024*16; i++){
        hash_table[i] = rand();
    }
}

static void init() {
    compile();
    init_hash_table();
    ctx_config_t *config = yices_new_config();
    yices_default_config_for_logic(config, "QF_BV");
    yices_context = yices_new_context(config);
    yices_free_config(config);
}

/*static void set_sim_log_name(uint sim_num) {
	sprintf(sim_file_name, "tests/sim_%04u.log", sim_num);
}

static void set_cnst_log_name(uint sim_num) {
	sprintf(cnst_file_name, "tests/constraints_%04u.ys", sim_num);
}

static void set_mem_log_name(uint sim_num) {
	sprintf(mem_file_name, "tests/data_%04u.mem", sim_num);
}*/

static void sim() {
	if(enable_sim_copy){
		string cmd = "cp data.mem " + string(mem_file_name);
		system(cmd.c_str());
		cmd = "vvp conc_run.vvp > " + string(sim_file_name);
		system(cmd.c_str());
	} else{
		system("vvp conc_run.vvp > sim.log");
	}
}

static void dump_yices_constraints(uint top_idx) {
	for(uint i=0; i<=top_idx; i++){
		constraint_t* cnst = constraints_stack[i];
		if(cnst->type != CNST_CLK){
            yices_assert_formula(yices_context, cnst->yices_term);
		}
	}
}

static bool check_yices_status(){
	smt_status_t status = yices_check_context(yices_context, NULL);
	if(status == STATUS_SAT){
		return true;
	}
	else if(status == STATUS_UNSAT){
		return false;
	}
	else{
		error("Syntax error in constraints");
		return false;
	}
}

static bool solve_constraints(uint clock) {
	
	bool is_sat = check_yices_status();
	if(is_sat){
		model_t *model = yices_get_model(yices_context, true);
		FILE *f_out = fopen("model.log", "w");
		yices_print_model(f_out, model);
		// yices_print_model(stdout, model);
		fclose(f_out);
		g_data.update_and_dump("model.log", g_data_mem);
		yices_free_model(model);
	}
	return is_sat;
}

static uint call_to_solver = 0;

bool compare_dist(br_cnst_t* a, br_cnst_t* b){
    if(a->br->block->distance == b->br->block->distance){
        return a->cnst->clock < b->cnst->clock;
    }
	return a->br->block->distance < b->br->block->distance;
}

bool compare_prob(br_cnst_t* a, br_cnst_t* b){
	if(a->br->branch_probability == b->br->branch_probability){
		return a->cnst->clock < b->cnst->clock;
	}
	return a->br->branch_probability > b->br->branch_probability;
}

static void update_path_taken(br_cnst_t* alt_path){
    uint alt_path_hash = alt_path->cnst->hash_value + hash_table[alt_path->br->id % 16384]*alt_path->cnst->index;
	path_hash_map.insert(alt_path_hash);
}

static bool is_path_taken(br_cnst_t* alt_path){
    uint alt_path_hash = alt_path->cnst->hash_value + hash_table[alt_path->br->id % 16384]*alt_path->cnst->index;
    return path_hash_map.find(alt_path_hash) != path_hash_map.end();
}

void simulate_build_stack() {
	//simulate
	sim();
	
	//reset variable versions to zero
	SMTSigCore::clear_all_versions();
	
	//build constraints stack
	constraints_stack.clear();
	build_stack();
}

static bool find_next_cfg(){

	const uint size = constraints_stack.size();
	vector<br_cnst_t*> branches;
	
	//create branches
	uint idx = 0;
	while(constraints_stack[idx]->clock == 0){
		idx++;
	}
	for(; idx < size; idx++){
		constraint_t* cnst = constraints_stack[idx];
		if(cnst->type == CNST_BRANCH){
			SMTBranch* br = dynamic_cast<SMTBranch*>(cnst->obj);
			assert(br);
            if(!br->is_dep){
                continue;
            }
			
			//add all selectable branches for sorting
			SMTBranchNode* node = br->parent_node;
			for(auto it:node->branch_list){
				//Yangdi: dont add if the smallest too far away
				if((it != br)){
				//if(it != br){
					br_cnst_t* alt_br = new br_cnst_t;
					alt_br->br = it;
					alt_br->cnst = cnst;
                    if(is_path_taken(alt_br) == false){
                        branches.push_back(alt_br);
                    }
				}
			}
		}
	}
	
	// //sort by distance
	// sort(branches.begin(), branches.end(), compare_dist);

	//sort by probability
	sort(branches.begin(), branches.end(), compare_prob);

	if(!user_select_branch){
		for(auto it:branches){
			uint clock = it->cnst->clock;
			//reset context
			yices_reset_context(yices_context);
			//insert initial assertion to zero for registers
			SMTSigCore::yices_insert_reg_init(yices_context);
			constraint_t** cnst = constraints_stack.data();
			while((*cnst)->clock != clock){
				if((*cnst)->type != CNST_CLK){			
					smt_yices_assert(yices_context, (*cnst)->yices_term, (*cnst)->obj);
				}
				cnst++;
			}	
			assert((*cnst)->type == CNST_CLK);
			cnst++;
			
			//restore version
			SMTSigCore::restore_versions(clock);
			const SMTProcess* target_process = it->cnst->obj->process;
			while(*cnst != it->cnst){
				const SMTProcess* process = (*cnst)->obj->process;
				if(!process->is_edge_triggered || (process == target_process)){
					term_t term = (*cnst)->obj->update_term();
					smt_yices_assert(yices_context, term, (*cnst)->obj);
				}
				cnst++;
			}

			//add mutated branch
			smt_yices_assert(yices_context, it->br->update_term(), it->br);
			

			call_to_solver++;
			check_satisfiability();
			if(solve_constraints(clock)){
				selected_branch = it->br;
				selected_clock = clock;
				//insert hash value even if potentially incorrect
				update_path_taken(it);
				if(!enable_error_check){
					it->br->set_covered_clk(sim_num+1, clock);
				}
				// Xiangchen: Adjust the probability
				if(is_new_block){
					SMTBranch::increase_probability(selected_branch);
				}else{
					if(sim_num > prob_num){
						SMTBranch::decrease_probability(selected_branch);
					}
				}
				SMTBranch::print_probability();
				selected_branch->k_permit_covered++;
				// Xiangchen: Because we use the probability to sort the branches
				// So we need not to consider the distance
				selected_branch->block->distance++;
				return true;
			}
		}	
	}else{
		// 打印所有分支
		std::cout << "Available branches:" << std::endl;
		for(size_t i = 0; i < branches.size(); ++i) {
			std::cout << "Index " << i << ": " << branches[i]->br->print() << std::endl;
		}

		// 获取用户输入
		std::cout << "Enter the index of the branch to select: ";
		size_t user_choice;
		std::cin >> user_choice;

		// 检查用户输入的有效性
		if(user_choice >= branches.size()) {
			std::cerr << "Invalid index selected." << std::endl;
			return false;
		}

		// 使用用户选择的分支
		auto selected_branch_it = branches.begin() + user_choice;

		// 用选择的分支代替原来的自动选择逻辑
		auto it = selected_branch_it;

		//printf("Trying %s\n", it->br->print().c_str());
		uint clock = (*it)->cnst->clock;
		//reset context
		yices_reset_context(yices_context);

		//insert initial assertion to zero for registers
		SMTSigCore::yices_insert_reg_init(yices_context);
		
		constraint_t** cnst = constraints_stack.data();
		while((*cnst)->clock != clock){
			if((*cnst)->type != CNST_CLK){			
				smt_yices_assert(yices_context, (*cnst)->yices_term, (*cnst)->obj);
			}
			cnst++;
		}	
		assert((*cnst)->type == CNST_CLK);
		cnst++;
		
		//restore version
		SMTSigCore::restore_versions(clock);
		const SMTProcess* target_process = (*it)->cnst->obj->process;
		while(*cnst != (*it)->cnst){
			const SMTProcess* process = (*cnst)->obj->process;
			if(!process->is_edge_triggered || (process == target_process)){
				term_t term = (*cnst)->obj->update_term();
				smt_yices_assert(yices_context, term, (*cnst)->obj);
			}
			cnst++;
		}

		//add mutated branch
		smt_yices_assert(yices_context, (*it)->br->update_term(), (*it)->br);
		

		call_to_solver++;
		check_satisfiability();
		if(solve_constraints(clock)){
			selected_branch = (*it)->br;
			selected_clock = clock;
			//insert hash value even if potentially incorrect
			update_path_taken((*it));
			if(!enable_error_check){
				(*it)->br->set_covered_clk(sim_num+1, clock);
			}
			// printf("[FOUND: CLOCK: %u, IDX: %u, DIST: %u, PROB: %f] %s", clock, selected_branch->list_idx, selected_branch->block->distance, selected_branch->branch_probability, it->br->print().c_str());
			// printf("[FOUND: CLOCK: %u, IDX: %u, PROB: %f] %s", clock, selected_branch->list_idx, selected_branch->branch_probability, it->br->print().c_str());
			
			// Xiangchen: Adjust the probability
			if(is_new_block){
				SMTBranch::increase_probability(selected_branch);
			}else{
				if(sim_num > prob_num){
					SMTBranch::decrease_probability(selected_branch);
				}else{
					SMTBranch::increase_probability(selected_branch);
				}
			}
			// SMTBranch::print_probability();
			selected_branch->k_permit_covered++;
			// Xiangchen: Because we use the probability to sort the branches
			// So we need not to consider the distance
			selected_branch->block->distance++;
			return true;
		}
	}
	return false;
}

static void check_satisfiability(){
    //reset context
    yices_reset_context(yices_context);

    //insert initial assertion to zero for registers
    SMTSigCore::yices_insert_reg_init(yices_context);

    //dump yices constraints
    dump_yices_constraints(constraints_stack.size()-1);

    //check status
    if(!check_yices_status()){
        smt_yices_dump_error();
        error("Simulation not satisfiable");
    }

	// Dump constraints
	if(true){
		// info("Dumping constraints");
		smt_yices_dump_error();
	}

}

static SMTPath* concolic_iteration(SMTPath* curr_path) {
    SMTPath* next_path = NULL;
    
	if(enable_error_check){
		//check satisfiability
		check_satisfiability();

		//check if selected branch is covered
		if(selected_branch){
			if(!selected_branch->is_covered_clk(selected_clock)){
				error("Selected branch is not covered by simulation");
			}
		}
	}
    
	if(find_next_cfg()){
        simulate_build_stack();
        //create path
        next_path = new SMTPath(g_data);
    }
	
	return next_path;
}

// Read targets from branch_id
uint select_target() {
	uint branch_id;
	FILE* f_branch_id = fopen("branch_id", "r");
	assert(f_branch_id);
	info("Reading Target id from file \"branch_id\"");
	while (fscanf(f_branch_id, "%d", &branch_id) == 1) {
		SMTAssign* assign = SMTAssign::get_assign(branch_id);
		assert(assign->assign_type == SMT_ASSIGN_BRANCH);
		SMTBasicBlock::add_target(assign->block);
		iter_count[assign->block] = 0;
	}
	fclose(f_branch_id);
	return SMTBasicBlock::target_num();
}

// Using multi target
void multi_coverage() {
	SMTPath *path = NULL;
	//reset variable versions to zero
	SMTSigCore::clear_all_versions();
	
	//edge realignment
	SMTBasicBlock::edge_realignment();
	SMTBasicBlock::update_all_distances();
	SMTProcess::make_circular();
	
	// ofstream f_cfg("cfg");
	// SMTBasicBlock::print_all(f_cfg);
	// f_cfg.close();
	//random simulations
	srand(1234);
	for(uint i=0; i<g_random_sim_num; i++){
		//generate random inputs
		g_data.generate();
		simulate_build_stack();   
		//save path
		path = new SMTPath(g_data);
		SMTBasicBlock::update_all_closest_paths(path, constraints_stack);
	}

	//remove covered
	SMTBasicBlock::remove_covered_targets(sim_num);
	sim_num = g_random_sim_num;
	while(!SMTBasicBlock::target_list.empty()){
		SMTBasicBlock::print_uncovered_targets();

		//For every target, it will give every branch a probability randomly
		// SMTBranch::random_probability();
		// For every target, it will give every branch a probability based on the distance
		SMTBranch::distance_probability();


		SMTBasicBlock* target = SMTBasicBlock::target_list.front();
		if (target->assign_list[0]->is_covered() || iter_count[target] >= total_limit) {
			SMTBasicBlock::target_list.pop_front();
			continue;
		}
		target->update_distance_from_adjacency_list();
		printf("\nTrying to cover %s", target->assign_list[0]->print().c_str());
		printf("Coverage rate: %f\n", SMTBranch::covered_branch_count / (float)SMTBranch::total_branch_count);

		iter_count[target] += iteration_limit;
		if (target->closest_path && target->closest_path != path) {
			path = target->closest_path;
			// printf("closest distance: %u\n", target->closest_path_distance);
			//update block distance from target's adjacency_list
			//Yangdi: Critical Error of first version
			//After selecting the path, signal clock versions need to be updated
			path->data.dump(g_data_mem);
			simulate_build_stack();
		}
		//do iterations till not covered or iteration limit reached
		selected_branch = NULL;
		uint start_iteration = sim_num;

		while((path = concolic_iteration(path))){
			
			sim_num++;  
			//erase covered target
			SMTBasicBlock::remove_covered_targets(sim_num);
			
			//check if target covered or iteration limit reached
			if(target->assign_list[0]->is_covered() || (sim_num - start_iteration) > iteration_limit){
				break;
			}
		}
		if (path)
			SMTBasicBlock::update_all_closest_paths(path, constraints_stack);

		if (!target->assign_list[0]->is_covered()) {
			target->closest_path_distance+=iteration_limit / 5;
			//generate random inputs
			for (uint i = 0; i < 2; i++) {
				g_data.generate();
				simulate_build_stack();   
				//save path
				path = new SMTPath(g_data);
				SMTBasicBlock::update_all_closest_paths(path, constraints_stack);
			}
			SMTBasicBlock::target_list.pop_front();
			SMTBasicBlock::target_list.push_back(target);
			target->dump_distances();
		}
	}
}

void start_concolic() {
	sim_num = 0;
	init();
	uint target_count = select_target();
	if(target_count == 0){
		error("No target specified");
	} else{
		info("Total targets: %d", target_count);
	}

	start_time = clock();

	// multi target to cover all
	multi_coverage();
	end_concolic();
}

void end_concolic(){
    clock_t end_time = clock();
	fflush(stdout);
    
    //ofstream report("report_cov.log");
    //report << "[TIME] " << difftime(end_time, start_time) << " sec\n";
	//report << "[TIME] " << (end_time - start_time)/double(CLOCKS_PER_SEC) << " sec\n";
	//report << "[ITER] " << sim_num << '\n';
    //report << "[SOLVER CALL] " << call_to_solver << '\n';
	//report << "[CNST BEFORE] " << total_constraints_before << '\n';
	//report << "[CNST AFTER] " << total_constraints_after << '\n';
	//report << "[TOTAL BRANCH] " << SMTBranch::total_branch_count << '\n';
	//report << "[COVERED BRANCH] " << SMTBranch::covered_branch_count << '\n';
    //SMTAssign::print_coverage(report);
    //report.close();
    
    //printf("[TIME] %.0lf sec\n", difftime(end_time, start_time));
	printf("[TIME] %.2lf sec\n", (end_time - start_time)/double(CLOCKS_PER_SEC));
    printf("[ITER] %u\n", sim_num);
	
    yices_free_context(yices_context);
    yices_exit();
    SMTFreeAll();
    exit(0);
}
