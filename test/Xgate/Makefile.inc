#Parameters that need to be changed depending on benchmark
IS_FLATTENED = false
TOP_MODULE 	= xgate_top
CLK_NAME 	= wbs_clk_i
RESET_NAME 	= wbs_rst_i
RESET_EDGE 	= 1
UNROLL_CYC	= 10
OBJ_SRC 	= ./src/xgate_risc.v \
./src/xgate_jtag.v \
./src/xgate_irq_encode.v \
./src/xgate_top.v \
./src/xgate_wbs_bus.v \
./src/xgate_wbm_bus.v \
./src/xgate_regs.v
