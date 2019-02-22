#Parameters that need to be changed depending on benchmark
TOP_MODULE 	= uart
CLK_NAME 	= sys_clk
RESET_NAME 	= sys_rst_l
RESET_EDGE 	= 0
UNROLL_CYC	= 1
OBJ_SRC 	= ./src/uart.v	\
			./src/u_rec.v	\
			./src/u_xmit.v

