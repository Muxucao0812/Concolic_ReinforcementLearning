#Parameters that need to be changed depending on benchmark
IS_FLATTENED = false
TOP_MODULE 	= top
CLK_NAME 	= clk
RESET_NAME 	= rst
RESET_EDGE 	= 1
UNROLL_CYC	= 10
TARGET		= 178681
OBJ_SRC 	= 	./src/aes_128.v		\
				./src/lfsr.v		\
				./src/round.v		\
				./src/table.v		\
				./src/top.v			\
				./src/Trojan_Trigger.v		\
				./src/TSC.v

