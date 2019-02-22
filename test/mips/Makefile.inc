#Parameters that need to be changed depending on benchmark
IS_FLATTENED = false
TOP_MODULE 	= mips_16_core_top
CLK_NAME 	= clk
RESET_NAME 	= rst
RESET_EDGE 	= 1
UNROLL_CYC	= 10
OBJ_SRC 	= ./src/alu.v \
	./src/EX_stage.v \
	./src/instruction_mem.v \
	./src/MEM_stage.v \
	./src/mips_16_defs.v \
	./src/ID_stage.v \
	./src/IF_stage.v \
	./src/WB_stage.v \
	./src/hazard_detection_unit.v \
	./src/mips_16_core_top.v \
	./src/data_mem.v \
	./src/register_file.v
