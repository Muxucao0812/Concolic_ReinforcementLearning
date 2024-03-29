#Parameters that need to be changed depending on benchmark
IS_FLATTENED = false
TOP_MODULE 	= or1200_top
CLK_NAME 	= clk_i
RESET_NAME 	= rst_i
RESET_EDGE 	= 1
UNROLL_CYC	= 10
OBJ_SRC 	= ./src/or1200_mem2reg.v \
./src/or1200_if.v \
./src/or1200_spram_1024x8.v \
./src/or1200_dc_fsm.v \
./src/or1200_dpram_256x32.v \
./src/or1200_sprs.v \
./src/or1200_dpram.v \
./src/or1200_dmmu_tlb.v \
./src/or1200_spram_1024x32_bw.v \
./src/or1200_defines.v \
./src/or1200_mult_mac.v \
./src/or1200_amultp2_32x32.v \
./src/or1200_fpu_pre_norm_div.v \
./src/or1200_lsu.v \
./src/or1200_cfgr.v \
./src/or1200_spram_128x32.v \
./src/or1200_iwb_biu.v \
./src/or1200_ic_top.v \
./src/or1200_alu.v \
./src/or1200_spram_2048x32.v \
./src/or1200_fpu_mul.v \
./src/or1200_tpram_32x32.v \
./src/or1200_ic_ram.v \
./src/timescale.v \
./src/or1200_spram_512x20.v \
./src/or1200_fpu_intfloat_conv.v \
./src/or1200_fpu_div.v \
./src/or1200_dc_tag.v \
./src/or1200_dmmu_top.v \
./src/or1200_fpu_post_norm_mul.v \
./src/or1200_spram_2048x32_bw.v \
./src/or1200_spram_256x21.v \
./src/or1200_except.v \
./src/or1200_gmultp2_32x32.v \
./src/or1200_ctrl.v \
./src/or1200_dc_ram.v \
./src/or1200_spram_64x24.v \
./src/or1200_rfram_generic.v \
./src/or1200_pm.v \
./src/or1200_reg2mem.v \
./src/or1200_cpu.v \
./src/or1200_xcv_ram32x8d.v \
./src/or1200_spram_64x22.v \
./src/or1200_ic_fsm.v \
./src/or1200_sb.v \
./src/or1200_fpu_pre_norm_mul.v \
./src/or1200_rf.v \
./src/or1200_fpu_post_norm_addsub.v \
./src/or1200_qmem_top.v \
./src/or1200_du.v \
./src/or1200_fpu_arith.v \
./src/or1200_dc_top.v \
./src/or1200_immu_top.v \
./src/or1200_spram_64x14.v \
./src/or1200_freeze.v \
./src/or1200_spram_1024x32.v \
./src/or1200_genpc.v \
./src/or1200_spram_32x24.v \
./src/or1200_spram_2048x8.v \
./src/or1200_fpu.v \
./src/or1200_spram.v \
./src/or1200_ic_tag.v \
./src/or1200_dpram_32x32.v \
./src/or1200_pic.v \
./src/or1200_wbmux.v \
./src/or1200_operandmuxes.v \
./src/or1200_tt.v \
./src/or1200_fpu_post_norm_intfloat_conv.v \
./src/or1200_spram_32_bw.v \
./src/or1200_fpu_intfloat_conv_except.v \
./src/or1200_wb_biu.v \
./src/or1200_top.v \
./src/or1200_fpu_pre_norm_addsub.v \
./src/or1200_sb_fifo.v \
./src/or1200_immu_tlb.v \
./src/or1200_fpu_addsub.v \
./src/or1200_fpu_fcmp.v \
./src/or1200_fpu_post_norm_div.v

