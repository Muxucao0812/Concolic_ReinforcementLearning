module wb_conmax_pri_dec(valid, pri_in, pri_out);

////////////////////////////////////////////////////////////////////
//
// Module Parameters
//

parameter [1:0]	pri_sel = 2'd0;

////////////////////////////////////////////////////////////////////
//
// Module IOs
//

input		valid;
input	[1:0]	pri_in;
output	[3:0]	pri_out;

////////////////////////////////////////////////////////////////////
//
// Local Wires
//

wire	[3:0]	pri_out;
reg	[3:0]	pri_out_d0;
reg	[3:0]	pri_out_d1;

////////////////////////////////////////////////////////////////////
//
// Priority Decoder
//

// 4 Priority Levels
always @(valid or pri_in)
	if(!valid)		pri_out_d1 = 4'b0001;
	else
	if(pri_in==2'h0)	pri_out_d1 = 4'b0001;
	else
	if(pri_in==2'h1)	pri_out_d1 = 4'b0010;
	else
	if(pri_in==2'h2)	pri_out_d1 = 4'b0100;
	else			pri_out_d1 = 4'b1000;

// 2 Priority Levels
always @(valid or pri_in)
	if(!valid)		pri_out_d0 = 4'b0001;
	else
	if(pri_in==2'h0)	pri_out_d0 = 4'b0001;
	else			pri_out_d0 = 4'b0010;

// Select Configured Priority

assign pri_out = (pri_sel==2'd0) ? 4'h0 : ( (pri_sel==1'd1) ? pri_out_d0 : pri_out_d1 );

endmodule

