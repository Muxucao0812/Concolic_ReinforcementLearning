/***********************************************************************
        Copyright (C) 2012,
        Virginia Polytechnic Institute & State University
        
        This verilog file is transformed from ITC 99 benchmark
        which is available from:
        http://www.cad.polito.it/downloads/tools/itc99.html.
        This verilog file was originally coverted by Mr. Min Li
        under the supervision of Dr. Michael S. Hsiao, in the
        Bradley Department of Electrical Engineering, VPI&SU, in
        2012. We made the conversion manually and verified it by 
        random simulation.

        This verilog file is released for research use only. This
        verilog file, or any derivative thereof, may not be
        reproduced nor used for any commercial product without the
        written permission of the authors.

        Mr. Min Li
        Research Assistant
        Bradley Department of Electrical Engineering
        Virginia Polytechnic Institute & State University
        Blacksburg, VA 24061

        Ph.: (540) 808-8129
        Fax: (540) 231-3362
        E-Mail: min.li@vt.edu
        Web: http://www.ece.vt.edu/mhsiao

***********************************************************************/

/*verilator lint_off WIDTH*/

module b07 ( clock, reset, start, punti_retta);

	input clock, reset, start;
    output reg [7:0] punti_retta;
	
	`define lung_mem		4'b1111

	parameter [2:0]  //synopsys enum state_info
					S_RESET   						=	3'b000,
					S_START   		   				= 	3'b001,
					S_LOAD_X 			       		=	3'b010,
					S_UPDATE_MAR 		       		=	3'b011,
					S_LOAD_Y 			       		=	3'b100,
					S_CALC_RETTA		       		=	3'b101,
					S_INCREMENTA 		       		=	3'b110;
					
	reg [2:0] /*synopsys enum state_info*/ stato; //synopsys state_vector stato	
	reg [7:0]  cont, mar, x, y , t;
	reg [7:0] mem [0:15];

always @(posedge clock)
	begin
		if (reset == 1'b1)
		begin
			punti_retta <= 0;
			cont <= 0;
			mar <= 0;
			x <= 0;
			y <= 0;
			t <= 0;

	        mem[0] <= 8'b00000001;
			mem[1] <= 8'b11111111;
			mem[2] <= 8'b00000000;
			mem[3] <= 8'b00000000;

			mem[4] <= 8'b00000000;
			mem[5] <= 8'b00000010;
			mem[6] <= 8'b00000000;
			mem[7] <= 8'b00000000;

			mem[8] <= 8'b00000000;
			mem[9] <= 8'b00000010;
			mem[10] <= 8'b11111111;
			mem[11] <= 8'b00000101;

			mem[12] <= 8'b00000000;
			mem[13] <= 8'b00000010;
//			mem[14] <= 8'b00000001;
			mem[14] <= 8'b00000000;
			mem[15] <= 8'b00000010;
			
			stato <= S_RESET;
		end
		else
		begin
			case (stato)
				S_RESET:
				begin
					stato <= S_START;
				end
				S_START:
				begin
					if (start == 1)
					begin
						cont <= 0;
						mar <= 0;
						stato <= S_LOAD_X;
					end
					else
					begin
						punti_retta <= 0;
						stato <= S_START;
					end
				end
				S_LOAD_X:
				begin
					x <= mem[mar];
					stato <= S_UPDATE_MAR;
				end
				S_UPDATE_MAR:
				begin
					mar <= mar + 1;
					t <= x + x;
					stato <= S_LOAD_Y;
				end
				S_LOAD_Y:
				begin
					y <= mem[mar];
					x <= x+t;
					stato <= S_CALC_RETTA;
				end
				S_CALC_RETTA:
				begin
					x<= x + y ;
					stato <= S_INCREMENTA;
				end
				S_INCREMENTA:
				begin
				   if (mar != {4'b0000, `lung_mem})
					begin
						if ( x == 8'b00000010) 
						begin
							cont <= cont +1;
							mar <= mar + 1;
							stato <= S_LOAD_X;							
						end
						else
						begin
							mar <= mar + 1;
							stato <= S_LOAD_X;
						end
					end
					else
					begin
						if (start == 0)
						begin
							if (x == 8'b00000010)
							begin
								punti_retta <= cont + 1;
								stato <= S_START;
							end
							else
							begin
								punti_retta <= cont;
								stato <= S_START;
							end
						end
						else
						begin
							stato <= S_INCREMENTA;
						end
					end
				end // case: S_INCREMENTA
              default:
                stato <= S_START; // min added! Should not reach!!
			endcase 
		end
	end
	


endmodule 
