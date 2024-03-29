#pragma once

#include "types.h"
#include "ivl_target.h"
#include "data_mem.h"
#include <cstdio>
#include <ctime>
using namespace std;
#include <unordered_map> 

//mannualy select
extern bool             user_select_branch;

//global variables
extern FILE*			g_out;
extern FILE*			g_prob;
extern FILE*			g_cover_result;
extern const char*	    g_prob_file;
extern const char*	    g_cover_result_file;
extern int				g_errors;
extern unsigned         g_ind;
extern unsigned         g_ind_incr;
extern ivl_design_t     g_design;
extern const char*      g_output_file;
extern const char*      g_tb_file;
extern const char*		g_data_mem;
extern const char*		g_data_mem_step;
extern const char*		g_data_state;
extern CTDataMem		g_data;
extern CTDataMem		g_data_step;
extern clock_t          start_time;


//Parameters
extern uint     		g_unroll;
extern uint     		g_step;
extern uint             g_sim_clk;
extern const char*      g_clock_sig_name;
extern const char*      g_reset_sig_name;
extern const char*      g_reset_edge_active;
extern uint             g_random_sim_num;
extern uint		        g_target_limit;
extern bool             g_is_new_block;
extern bool             g_distance_decrease;

//Experiment result handler
extern const char*      g_name_exp_res;
/*! \brief Global experiment result file*/
extern FILE*            g_exp_res;


//Configuration declarations
//#define conc_flush(X)   fflush(X)
#define conc_flush(X) 
extern const bool		enable_error_check;
extern const bool		enable_obs_padding;
extern const bool		enable_yices_debug;
extern const bool		enable_sim_copy;

//Utility functions 
extern void error(const char *fmt, ...);
extern void info(const char *fmt, ...);


//Qlearn parameters
extern double epsilon_qlearn ; // 探索率
extern double alpha_qlearn ; // 学习率
extern double gamma_qlearn ; // 折扣因子
extern std::unordered_map<std::string, std::vector<double>> q_table; //q_table




