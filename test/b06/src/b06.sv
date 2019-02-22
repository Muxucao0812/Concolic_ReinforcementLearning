/*
entity b06 is
 port (cc_mux : out bit_vector(2 downto 1);
	   eql: in bit;
	   uscite: out bit_vector(2 downto 1);
	   clock: in bit;
	   enable_count: out bit;
	   ackout: out bit;
	   reset: in bit; 
	   cont_eql: in bit);

end b06;

architecture BEHAV of b06 is

*/
module b06(
	input eql,
	input clock,
	input reset,
	input cont_eql,

	output reg [1:0] cc_mux,
	output reg [1:0] uscite,
	output reg enable_count,
	output reg ackout
	);

	reg [2:0] state ;
	parameter   s_init = 3'b000;
	parameter   s_wait = 3'b001;
	parameter   s_enin = 3'b010;
	parameter s_enin_w = 3'b011;
	parameter   s_intr = 3'b100;
	parameter s_intr_1 = 3'b101;
	parameter s_intr_w = 3'b110;


		
	parameter   cc_nop = 2'b01;
	parameter  cc_enin = 2'b01;
	parameter  cc_intr = 2'b10;
	parameter cc_ackin = 2'b11;
	parameter out_norm = 2'b01; 
	
	reg target;	
		 
	always@(posedge clock)
		if(reset == 1'b1)
			begin
				target <= 0;
				state <= s_init;
				cc_mux <= 2'b00;
				enable_count <= 1'b0;
				ackout <= 1'b0;
				uscite <= 2'b00;
			end
		else
		begin
			if(cont_eql == 1'b1)
			begin
				ackout <= 1'b0;
				enable_count <= 1'b0;
			end
			else
			begin
				ackout <= 1'b1;
				enable_count <= 1'b1;
			end
		
			case(state)
			s_init:
			begin
				cc_mux <= cc_enin;
				uscite <= out_norm;
				state <= s_wait;
			end
			s_wait:
				if(eql == 1'b1)
				begin
					uscite <= 2'b00;
					cc_mux <= cc_ackin;
					state <= s_enin;
				end
				else
				begin
					uscite <= out_norm;
					cc_mux <= cc_intr;
					state <= s_intr_1;
				end
			s_intr_1:
				if(eql == 1'b1)
					begin
						uscite <=2'b00;
						cc_mux <= cc_ackin;
						state <= s_intr;
					end
				else
				begin
					uscite <= out_norm;
					cc_mux <= cc_enin;
					state <= s_wait;
				end

			s_enin:
				if(eql == 1'b1)
					begin
						uscite <= 2'b00;
						cc_mux <= cc_ackin;
						state <= s_enin;
					end
				else
					begin
						uscite <= 2'b01;
						ackout <= 1'b1;
						enable_count <= 1'b1;
						cc_mux <= cc_enin;
						state <= s_enin_w;
					end

			s_enin_w:
				if(eql == 1'b1)
					begin
						uscite <= 2'b01;
						cc_mux <= cc_enin;
						state <= s_enin_w;
					end
				else
					begin
						uscite <= out_norm;
						cc_mux <= cc_enin;
						state <= s_wait;
					end

			s_intr:
				if(eql == 1'b1)
					begin
						uscite <= 2'b00;
						cc_mux <= cc_ackin;
						state <= s_intr;
					end
				 else
					begin
						uscite <= 2'b11;
						cc_mux <= cc_intr;
						state <= s_intr_w;
					end

			s_intr_w:
				if(eql == 1'b1)
					begin
						target <= 1;
						uscite <= 2'b11;
						cc_mux <= cc_intr;
						state <= s_intr_w;
					end
				else
					begin
						uscite <= out_norm;
						cc_mux <= cc_enin;
						state <= s_wait;
					end

		endcase
		end
		
		assert property	(not((reset == 0) && (target == 1)));
		
endmodule

