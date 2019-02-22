module wb_conmax_pri_enc(
		valid,
		pri0, pri1, pri2, pri3,
		pri4, pri5, pri6, pri7,
		pri_out
		);

////////////////////////////////////////////////////////////////////
//
// Module Parameters
//

parameter	[1:0]	pri_sel = 2'd0;

////////////////////////////////////////////////////////////////////
//
// Module IOs
//

input	[7:0]	valid;
input	[1:0]	pri0, pri1, pri2, pri3;
input	[1:0]	pri4, pri5, pri6, pri7;
output	[1:0]	pri_out;

////////////////////////////////////////////////////////////////////
//
// Local Wires
//

wire	[3:0]	pri0_out, pri1_out, pri2_out, pri3_out;
wire	[3:0]	pri4_out, pri5_out, pri6_out, pri7_out;
wire	[3:0]	pri_out_tmp;
reg	[1:0]	pri_out0, pri_out1;
wire	[1:0]	pri_out;

////////////////////////////////////////////////////////////////////
//
// Priority Decoders
//

wb_conmax_pri_dec #(pri_sel) pd0(
		.valid(		valid[0]	),
		.pri_in(	pri0		),
		.pri_out(	pri0_out	)
		);


wb_conmax_pri_dec #(pri_sel) pd1(
		.valid(		valid[1]	),
		.pri_in(	pri1		),
		.pri_out(	pri1_out	)
		);

wb_conmax_pri_dec #(pri_sel) pd2(
		.valid(		valid[2]	),
		.pri_in(	pri2		),
		.pri_out(	pri2_out	)
		);

wb_conmax_pri_dec #(pri_sel) pd3(
		.valid(		valid[3]	),
		.pri_in(	pri3		),
		.pri_out(	pri3_out	)
		);

wb_conmax_pri_dec #(pri_sel) pd4(
		.valid(		valid[4]	),
		.pri_in(	pri4		),
		.pri_out(	pri4_out	)
		);

wb_conmax_pri_dec #(pri_sel) pd5(
		.valid(		valid[5]	),
		.pri_in(	pri5		),
		.pri_out(	pri5_out	)
		);

wb_conmax_pri_dec #(pri_sel) pd6(
		.valid(		valid[6]	),
		.pri_in(	pri6		),
		.pri_out(	pri6_out	)
		);

wb_conmax_pri_dec #(pri_sel) pd7(
		.valid(		valid[7]	),
		.pri_in(	pri7		),
		.pri_out(	pri7_out	)
		);

////////////////////////////////////////////////////////////////////
//
// Priority Encoding
//

assign pri_out_tmp =	pri0_out | pri1_out | pri2_out | pri3_out |
			pri4_out | pri5_out | pri6_out | pri7_out;

// 4 Priority Levels
always @(pri_out_tmp)
	if(pri_out_tmp[3])	pri_out1 = 2'h3;
	else
	if(pri_out_tmp[2])	pri_out1 = 2'h2;
	else
	if(pri_out_tmp[1])	pri_out1 = 2'h1;
	else			pri_out1 = 2'h0;

// 2 Priority Levels
always @(pri_out_tmp)
	if(pri_out_tmp[1])	pri_out0 = 2'h1;
	else			pri_out0 = 2'h0;

////////////////////////////////////////////////////////////////////
//
// Final Priority Output
//

// Select configured priority

assign pri_out = (pri_sel==2'd0) ? 2'h0 : ( (pri_sel==2'd1) ? pri_out0 : pri_out1 );

endmodule


