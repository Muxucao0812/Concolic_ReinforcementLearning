#include "ivl_target.h"
#include "smt_lib.h"
#include "data_mem.h"
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>

using namespace std;

//manualy select
bool            user_select_branch = false;


//global variables
FILE*			g_out = NULL;
FILE*			g_prob = NULL;
FILE*			g_uncovered_targets = NULL;
const char*	    g_prob_file = "prob.txt";
const char*	    g_uncovered_targets_file = "uncovered_targets.txt";
int				g_errors = 0;
unsigned		g_ind = 0;
unsigned		g_ind_incr = 4;
ivl_design_t	g_design = 0;
const char*     g_output_file = "conquest_dut.v";
const char*     g_tb_file = "conquest_tb.v";
const char*		g_data_mem = "data.mem";
const char*		g_data_mem_step = "data_step.mem";
const char*		g_data_state = "data.state";
CTDataMem		g_data;
CTDataMem		g_data_step;
clock_t         start_time;
bool            g_is_new_block = false;       
//Parameters
uint			g_unroll;
uint			g_step;
uint            g_sim_clk = 10;
const char*     g_clock_sig_name;
const char*     g_reset_sig_name;
const char*     g_reset_edge_active;
uint            g_random_sim_num;
uint            prob_num = 50;

/*! \brief The name of experiment result file*/
const char*      g_name_exp_res = "exp_res.txt";

/*! \brief Global experiment result file*/
FILE*            g_exp_res;