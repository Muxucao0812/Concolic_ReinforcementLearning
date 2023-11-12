#Parameters that need to be changed depending on benchmark
IS_FLATTENED = false
TOP_MODULE 	= spc 
CLK_NAME 	= gclk
RESET_NAME 	= reset
RESET_EDGE 	= 1
UNROLL_CYC	= 10
OBJ_SRC 	= ./src/spc_mb1_ctl.v \
./src/spc.v \
./src/spc_mb0_ctl.v \
./src/dmo_dp.v \
./src/spc_rep1_dp.v \
./src/spc_msf0_dp.v \
./src/spc_msf1_dp.v \
./src/spc_mb2_ctl.v \
./src/spc_lb_ctl.v
