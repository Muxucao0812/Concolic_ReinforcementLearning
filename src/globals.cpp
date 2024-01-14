#include "ivl_target.h"
#include "smt_lib.h"
#include "data_mem.h"
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>

using namespace std;

//mannualy select
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
const char*		g_data_state = "data.state";
CTDataMem		g_data;
clock_t         start_time;
bool            is_new_block = false;       
//Parameters
uint			g_unroll;
uint			g_step;
const char*     g_clock_sig_name;
const char*     g_reset_sig_name;
const char*     g_reset_edge_active;
uint            g_random_sim_num = 3;
uint            prob_num = 50;
