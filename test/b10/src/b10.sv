
module b10(
	input	r_button ,
	input	g_button ,
	input	key      ,
	input	start    ,
	input	reset    ,
	input	test     ,
	output	reg cts      ,
	output	reg ctr      ,
	input	rts      ,
	input	rtr      ,
	input	clock    ,
	input [3:0]	v_in ,
	output reg [3:0]	v_out 
);

reg [3:0] stato ;
reg voto0,voto1,voto2,voto3;
reg [3:0] sign ;
reg last_g,last_r;


parameter STARTUP  = 4'b0000;
parameter STANDBY  = 4'b0001;
parameter GET_IN   = 4'b0010;
parameter START_TX = 4'b0011;
parameter SEND     = 4'b0100;
parameter TX_2_RX  = 4'b0101;
parameter RECEIVE  = 4'b0110;
parameter RX_2_TX  = 4'b0111;
parameter END_TX   = 4'b1000;
parameter TEST_1   = 4'b1001;
parameter TEST_2   = 4'b1010;

reg target;

always@(posedge clock )
	if(reset == 1'b1)
		begin
			target <= 0;
			stato  <=  STARTUP;
			voto0  <=  1'b0;
			voto1  <=  1'b0;
			voto2  <=  1'b0;
			voto3  <=  1'b0;
			sign  <=  4'b0000;
			last_g  <=  1'b0;
			last_r  <=  1'b0;
			cts <= 1'b0;
			ctr <= 1'b0;
			v_out <= 4'b0000;
		end
	else
		case(stato)

		STARTUP:
			begin
					voto0  <=  1'b0;
				    voto1  <=  1'b0;
				    voto2  <=  1'b0;
				    voto3  <=  1'b0;
				    cts   <= 1'b0     ;
				    ctr   <= 1'b0     ;
				    if ( test == 1'b0 )
					begin
				       sign   <=  4'b0000 ;
				       stato  <=  TEST_1 ;
					end
				    else
					begin
				       voto0  <=  1'b0;
				       voto1  <=  1'b0;
				       voto2  <=  1'b0;
				       voto3  <=  1'b0;
				       stato  <=  STANDBY ;
					end
			end
			STANDBY:
			begin
					if(start == 1'b1)
					begin
				       voto0  <=  1'b0;
				       voto1  <=  1'b0;
				       voto2  <=  1'b0;
				       voto3  <=  1'b0;
				       stato  <=  GET_IN;
					end
				    if(rtr == 1'b1)
				       cts   <= 1'b1;
				    if(rtr == 1'b0)
				       cts <= 1'b0 ;
			end
			GET_IN:
				if(start == 1'b0)
					stato  <=  START_TX ;
				else if(key == 1'b1)
					begin
						voto0  <=  key ;
						if ((( g_button ^ last_g ) & ( g_button )) == 1'b1 )
							voto1  <=  ~voto1 ;
						if ((( r_button ^ last_r ) & ( r_button )) == 1'b1)
							voto2  <=  ~voto2 ;
						last_g  <=  g_button ;
						last_r  <=  r_button ;
					end
				else
					begin
				       voto0  <=  1'b0;
				       voto1  <=  1'b0;
				       voto2  <=  1'b0;
				       voto3  <=  1'b0;
					end
			START_TX:
			begin
				voto3  <=  voto0 ^ (voto1 ^ voto2);
				stato  <=  SEND;
				voto0  <=  1'b0;
			end
			SEND:
				if(rtr == 1'b1)
				begin
					v_out <= {voto3, voto2, voto1, voto0};
					//v_out[0] <= voto0 ;
					//v_out[1] <= voto1 ;
					//v_out[2] <= voto2 ;
					//v_out[3] <= voto3 ;
					cts <= 1'b1 ;
					if (voto0 == 1'b0 && voto1 == 1'b1 && voto2 == 1'b1 && voto3 == 1'b0)
					   stato  <=  END_TX ;
					else
					   stato  <=  TX_2_RX ;
				end
		   TX_2_RX:
				if(rts == 1'b0)
				begin
				       ctr <= 1'b1 ;
				       stato  <=  RECEIVE ;
				end
			RECEIVE:
				if(rts == 1'b1)
				begin
				       voto0  <=  v_in[0] ;
				       voto1  <=  v_in[1] ;
				       voto2  <=  v_in[2] ;
				       voto3  <=  v_in[3] ;
				       ctr <= 1'b0 ;
				       stato  <=  RX_2_TX ;
				end
			RX_2_TX:
				if(rtr == 1'b0)
				begin
				    cts <= 1'b0 ;
				    stato  <=  SEND ;
				end
			END_TX:
				if(rtr == 1'b0)
				begin
				    cts <= 1'b0 ;
                    stato  <=  STANDBY ;
				end

			TEST_1:
				begin
					voto0  <=  v_in[0] ;
				    voto1  <=  v_in[1] ;
				    voto2  <=  v_in[2] ;
				    voto3  <=  v_in[3] ;
				    sign  <=  4'b1000;
				    if(voto0 == 1'b1 && voto1 == 1'b1 && voto2 == 1'b1 && voto3 == 1'b1) begin
						stato  <=  TEST_2 ;
						
						end
						else begin
						target = 1;
						end
						
				end
			TEST_2:
				begin
					voto0  <=  1'b1 ^ sign[0] ;
				    voto0  <=  1'b0 ^ sign[1] ;
				    voto0  <=  1'b0 ^ sign[2] ;
				    voto0  <=  1'b1 ^ sign[3] ;
				    stato  <=  SEND ;
				end
		endcase
	
	assert property	(not((reset == 0) && (target == 1)));
	
endmodule
