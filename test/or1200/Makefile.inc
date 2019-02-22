#Parameters that need to be changed depending on benchmark
IS_FLATTENED = false
TOP_MODULE 	= soc_top
CLK_NAME 	= sys_clk
RESET_NAME 	= sys_rst
RESET_EDGE 	= 1
UNROLL_CYC	= 10
OBJ_SRC 	= ./src/soc_registers.v \
./src/soc_top.v \
./src/soc_adv_dbg.v \
./src/soc_defines.v \
./src/soc_boot.v \
./src/soc_system.v \
./src/soc_mem_bank_3.v \
./src/soc_mem_bank_2.v \
./src/amf_sld_virtual_jtag.v \
./src/soc_peripherals.v \
./src/soc_mem_bank_1.v \
./src/soc_gpio.v \
./src/soc_ram.v \
./src/altera_virtual_jtag.v
