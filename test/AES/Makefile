OBJ_FLAT = flattened.v
OBJ_INST = conquest_dut.v
RANDOM_SIM = 3

FLATTEN_ROOT = /home/yangdi/tools/designplayer-shell/10APR2018


.PHONY: all clean

ifeq ($(IS_FLATTENED),true)
all:
	@iverilog -f files.f -o $(OBJ_INST) -t conquestMulti \
		-ptb=conquest_tb.v -pclk=$(CLK_NAME) -preset=$(RESET_NAME) -preset_edge=$(RESET_EDGE) \
		-punroll=$(UNROLL_CYC) -prandom_sim=$(RANDOM_SIM)
	
else
all: $(OBJ_FLAT)
	@iverilog $(OBJ_FLAT) -o $(OBJ_INST) -t conquestMulti \
		-ptb=conquest_tb.v -pclk=$(CLK_NAME) -preset=$(RESET_NAME) -preset_edge=$(RESET_EDGE) \
		-punroll=$(UNROLL_CYC) -prandom_sim=$(RANDOM_SIM)

$(OBJ_FLAT): $(OBJ_SRC)
	@flattenverilog -f files.f -top $(TOP_MODULE) -quiet -nobanner -out $(OBJ_FLAT) -disable_assignment_merge
	@rm -rf flattenverilog.log
endif


clean:
	@rm -rf *.ys
	@rm -rf *.v
	@rm -rf *.out
	@rm -rf cfg
	@rm -rf data.mem
	@rm -rf data_raw.mem
	@rm -rf *.txt
	@rm -rf *.vvp
	@rm -rf *.log
	@echo "--------------CLEANED---------------"
