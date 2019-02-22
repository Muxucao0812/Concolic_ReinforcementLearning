module wb_conmax_msel(
		clk_i, rst_i,
		conf, req, sel, next
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

input		clk_i, rst_i;
input	[15:0]	conf;
input	[7:0]	req;
output	[2:0]	sel;
input		next;

////////////////////////////////////////////////////////////////////
//
// Local Wires
//

wire	[1:0]	pri0, pri1, pri2, pri3;
wire	[1:0]	pri4, pri5, pri6, pri7;
wire	[1:0]	pri_out_d;
reg	[1:0]	pri_out;

wire	[7:0]	req_p0, req_p1, req_p2, req_p3;
wire	[2:0]	gnt_p0, gnt_p1, gnt_p2, gnt_p3;

reg	[2:0]	sel1, sel2;
wire	[2:0]	sel;

////////////////////////////////////////////////////////////////////
//
// Priority Select logic
//

assign pri0 = {(pri_sel == 2'd2) ? conf[1] : 1'b0, (pri_sel == 2'd0) ? 1'b0 : conf[0]};
assign pri1 = {(pri_sel == 2'd2) ? conf[3] : 1'b0, (pri_sel == 2'd0) ? 1'b0 : conf[2]};
assign pri2 = {(pri_sel == 2'd2) ? conf[5] : 1'b0, (pri_sel == 2'd0) ? 1'b0 : conf[4]};
assign pri3 = {(pri_sel == 2'd2) ? conf[7] : 1'b0, (pri_sel == 2'd0) ? 1'b0 : conf[6]};
assign pri4 = {(pri_sel == 2'd2) ? conf[9] : 1'b0, (pri_sel == 2'd0) ? 1'b0 : conf[8]};
assign pri5 = {(pri_sel == 2'd2) ? conf[11] : 1'b0, (pri_sel == 2'd0) ? 1'b0 : conf[10]};
assign pri6 = {(pri_sel == 2'd2) ? conf[13] : 1'b0, (pri_sel == 2'd0) ? 1'b0 : conf[12]};
assign pri7 = {(pri_sel == 2'd2) ? conf[15] : 1'b0, (pri_sel == 2'd0) ? 1'b0 : conf[14]};

// Priority Encoder
wb_conmax_pri_enc #(pri_sel) pri_enc(
	.valid(		req		),
	.pri0(		pri0		),
	.pri1(		pri1		),
	.pri2(		pri2		),
	.pri3(		pri3		),
	.pri4(		pri4		),
	.pri5(		pri5		),
	.pri6(		pri6		),
	.pri7(		pri7		),
	.pri_out(	pri_out_d	)
	);

always @(posedge clk_i)
	if(rst_i)	pri_out <= #1 2'h0;
	else
	if(next)	pri_out <= #1 pri_out_d;

////////////////////////////////////////////////////////////////////
//
// Arbiters
//
assign req_p0 = {req[7] & (pri7 == 2'd0), req[6] & (pri6 == 2'd0), req[5] & (pri5 == 2'd0), req[4] & (pri4 == 2'd0), req[3] & (pri3 == 2'd0), req[2] & (pri2 == 2'd0), req[1] & (pri1 == 2'd0), req[0] & (pri0 == 2'd0)};
assign req_p1 = {req[7] & (pri7 == 2'd1), req[6] & (pri6 == 2'd1), req[5] & (pri5 == 2'd1), req[4] & (pri4 == 2'd1), req[3] & (pri3 == 2'd1), req[2] & (pri2 == 2'd1), req[1] & (pri1 == 2'd1), req[0] & (pri0 == 2'd1)};
assign req_p2 = {req[7] & (pri7 == 2'd2), req[6] & (pri6 == 2'd2), req[5] & (pri5 == 2'd2), req[4] & (pri4 == 2'd2), req[3] & (pri3 == 2'd2), req[2] & (pri2 == 2'd2), req[1] & (pri1 == 2'd2), req[0] & (pri0 == 2'd2)};
assign req_p3 = {req[7] & (pri7 == 2'd3), req[6] & (pri6 == 2'd3), req[5] & (pri5 == 2'd3), req[4] & (pri4 == 2'd3), req[3] & (pri3 == 2'd3), req[2] & (pri2 == 2'd3), req[1] & (pri1 == 2'd3), req[0] & (pri0 == 2'd3)};

wb_conmax_arb arb0(
	.clk(		clk_i		),
	.rst(		rst_i		),
	.req(		req_p0		),
	.gnt(		gnt_p0		),
	.next(		1'b0		)
	);

wb_conmax_arb arb1(
	.clk(		clk_i		),
	.rst(		rst_i		),
	.req(		req_p1		),
	.gnt(		gnt_p1		),
	.next(		1'b0		)
	);

wb_conmax_arb arb2(
	.clk(		clk_i		),
	.rst(		rst_i		),
	.req(		req_p2		),
	.gnt(		gnt_p2		),
	.next(		1'b0		)
	);

wb_conmax_arb arb3(
	.clk(		clk_i		),
	.rst(		rst_i		),
	.req(		req_p3		),
	.gnt(		gnt_p3		),
	.next(		1'b0		)
	);

////////////////////////////////////////////////////////////////////
//
// Final Master Select
//

always @(pri_out or gnt_p0 or gnt_p1)
	if(pri_out[0])	sel1 = gnt_p1;
	else		sel1 = gnt_p0;


always @(pri_out or gnt_p0 or gnt_p1 or gnt_p2 or gnt_p3)
	case(pri_out)
	   2'd0: sel2 = gnt_p0;
	   2'd1: sel2 = gnt_p1;
	   2'd2: sel2 = gnt_p2;
	   2'd3: sel2 = gnt_p3;
	endcase


assign sel = (pri_sel==2'd0) ? gnt_p0 : ( (pri_sel==2'd1) ? sel1 : sel2 );

endmodule

