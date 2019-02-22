#Parameters that need to be changed depending on benchmark
IS_FLATTENED = false
TOP_MODULE 	= wb_conmax_top
CLK_NAME 	= clk_i
RESET_NAME 	= rst_i
RESET_EDGE 	= 1
UNROLL_CYC	= 10
OBJ_SRC 	= ./src/wb_conmax_arb.v			\
			./src/wb_conmax_master_if.v		\
			./src/wb_conmax_msel.v			\
			./src/wb_conmax_pri_dec.v		\
			./src/wb_conmax_pri_enc.v		\
			./src/wb_conmax_rf.v			\
			./src/wb_conmax_slave_if.v		\
			./src/wb_conmax_top.v

