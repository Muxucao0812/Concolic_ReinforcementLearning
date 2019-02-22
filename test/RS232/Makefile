CONC_TEST_DIR = tests

OBJ_FLAT = flattened.v
OBJ_INST = conquest_dut.v
ENABLE_ALL = true
RANDOM_SIM = 5

FLATTEN_ROOT = /home/yangdi/tools/designplayer-shell/10APR2018


.PHONY: all clean

ifeq ($(IS_FLATTENED),true)
all:
	@rm -rf $(CONC_TEST_DIR)
	@mkdir -p $(CONC_TEST_DIR)
	@iverilog_dev -f files.f -o $(OBJ_INST) -t conquestMulti \
		-ptb=conquest_tb.v -pclk=$(CLK_NAME) -preset=$(RESET_NAME) -preset_edge=$(RESET_EDGE) \
		-punroll=$(UNROLL_CYC) -penable_all=$(ENABLE_ALL) -prandom_sim=$(RANDOM_SIM)
	
else
all: $(OBJ_FLAT)
	@rm -rf $(CONC_TEST_DIR)
	@mkdir -p $(CONC_TEST_DIR)
	@iverilog_dev $(OBJ_FLAT) -o $(OBJ_INST) -t conquestMulti \
		-ptb=conquest_tb.v -pclk=$(CLK_NAME) -preset=$(RESET_NAME) -preset_edge=$(RESET_EDGE) \
		-punroll=$(UNROLL_CYC) -penable_all=$(ENABLE_ALL) -prandom_sim=$(RANDOM_SIM)

$(OBJ_FLAT): $(OBJ_SRC)
	@flattenverilog -f files.f -top $(TOP_MODULE) -quiet -nobanner -out $(OBJ_FLAT) -remove_unused_nets
	@rm -rf flattenverilog.log
endif


clean:
	@rm -rf $(CONC_TEST_DIR)
	@rm -rf *.ys
	@rm -rf *.v
	@rm -rf *.out
	@rm -rf data.mem
	@rm -rf *.txt
	@rm -rf *.vvp
	@rm -rf *.log
	@echo "--------------CLEANED---------------"
