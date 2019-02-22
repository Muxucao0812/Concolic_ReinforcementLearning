module b01 (
	input clock, 
	input  line1, 
	input line2,
	input reset,

	output reg outp,
	output reg overflw
);

reg [2:0] stato;

parameter   a = 3'b0;
parameter   b = 3'b001;
parameter   c = 3'b010;
parameter   e = 3'b011;
parameter   f = 3'b100;
parameter   g = 3'b101;
parameter wf0 = 3'b110;
parameter wf1 = 3'b111;

reg target;

always@(posedge clock)
    if(reset==1'b1) 
		begin
			target <= 0;
			stato <= a;
			outp <= 1'b0;
			overflw<=1'b0;
		end
    else 
		case(stato)
		a:
			begin
				if(line1 == 1'b1 && line2==1'b1) begin
					stato <= f;
				end
				else begin
					stato <= b ;
					end
				outp <= line1 | line2;
				overflw <= 1'b0;
			end
		e:
			begin
				if(line1 == 1'b1 && line2==1'b1 ) begin
					stato <= f;
				end
				else begin
					stato <= b ;
				end
				outp <= line1 | line2;
				overflw <= 1'b1;
			end
        b:   
			begin
                if(line1 == 1'b1 && line2 == 1'b1) begin
					stato <= g ;
				end
                else begin 
                    stato <= c;
				end
				outp <= line1 | line2;
				overflw <= 1'b0 ;
			end
        f:
			begin
                if( line1 == 1'b1 || line2 == 1'b1) begin
                    stato <= g;
				end
                else begin
                	target = 1;
                    stato <= c;
				end
				outp <= ~(line1 ^ line2);
				overflw <= 1'b0 ;
			end
        c:  
			begin
                if(line1 == 1'b1 && line2 == 1'b1 ) begin
                    stato <= wf1;
				end
                else begin
                    stato <= wf0;
				end
				outp <= line1 | line2;
				overflw <= 1'b0 ;
			end
        g: 
			begin
                if( line1 == 1'b1 || line2 ==1'b1) begin
					stato <= wf1;
				end
                else begin
					stato <= wf0;
            	end
				outp <= ~(line1 | line2);
				overflw <= 1'b0 ;
			end
        wf0:
			begin
                if( line1==1'b1 && line2==1'b1 ) begin
                    stato <= e;
				end
                else begin
                    stato <= a;
				end
				outp <= line1 | line2;
				overflw <= 1'b0 ;
			end
        wf1:
			begin
                if( line1==1'b1 || line2==1'b1 ) begin
                    stato <= e;
				end
                else begin
                    stato <= a;
				end
				outp <= ~(line1 | line2);
				overflw <= 1'b0 ;
			end
        endcase

	assert property	(not((reset == 0) && (target == 1)));

endmodule
