module b12(
	       input            clock,
	       input            reset,
	       input            start,
	       input [3:0]      k,
	       output reg           nloss,
	       output reg [3:0]      nl,
	       output reg           speaker
	       );

   parameter RED = 0;
   parameter GREEN = 1;
   parameter YELLOW = 2;
   parameter BLUE = 3;

   parameter LED_ON = 1;
   parameter LED_OFF = 0;

   parameter PLAY_ON = 1;
   parameter PLAY_OFF = 0;

   parameter KEY_ON = 1;

   parameter NUM_KEY = 4;
   parameter COD_COLOR = 2;
   parameter COD_SOUND = 3;




   parameter SIZE_ADDRESS = 5;
   parameter SIZE_MEM = 2**SIZE_ADDRESS;

   parameter COUNT_KEY = 33;
   parameter COUNT_SEQ = 33;
   parameter DEC_SEQ = 1;
   parameter COUNT_FIN = 8;

   parameter ERROR_TONE = 1;
   parameter RED_TONE = 2;
   parameter GREEN_TONE = 3;
   parameter YELLOW_TONE = 4;
   parameter BLUE_TONE = 5;
   parameter WIN_TONE = 6;

   reg [0:0]                     wr;
   reg [(SIZE_ADDRESS-1):0] address;
   //reg[SIZE_ADDRESS:0] address;
   reg [(COD_COLOR-1):0]    data_in;
   reg [(COD_COLOR-1):0]    data_out;
   reg [(COD_COLOR-1):0]    num;
   reg [(COD_SOUND-1):0]    sound;
   // reg[((2**COD_COLOR)-1):0] data_in;
   // reg[((2**COD_COLOR)-1):0] data_out;
   // reg[((2**COD_COLOR)-1):0] num;
   // reg[((2**COD_SOUND)-1):0] sound;
   reg [0:0]                   play;

   //reg[3:0] counter;

   //reg[((2**COD_COLOR)-1):0] count;

   //reg[((2**COD_COLOR)-1):0] memory [0:(SIZE_MEM-1)];
   //reg[SIZE_ADDRESS:0] mar;
   reg [0:0] s;
   reg [2:0] counter;

   always @ (posedge clock )
   begin
	    if (reset) //0
	      begin
		     s = 0;
		     speaker <= 0;
		     counter = 0;
	      end
	    else //22
	      begin
		     if (play)//20
		       begin
			      case (sound) 
				    0: //3
				      begin
					     if (counter > RED_TONE) // > 0 //1
					       begin
						      s = ~s;
						      counter = 0;
						      speaker <= s;
					       end
					     else //2
						   counter = counter + 1;
				      end
				    
				    1: //6
				      begin
					     if (counter > GREEN_TONE) // > 1 //4
					       begin
						      s = ~s;
						      speaker <= s;
						      counter = 0;
					       end
					     else //5
						   counter = counter + 1;
				      end
				    
				    2: //9
				      begin 
					     if (counter > YELLOW_TONE) // > 2 //7
					       begin
						      s = ~s;
						      speaker <= s;
						      counter = 0;
					       end
					     else //8
						   counter = counter + 1;
				      end
				    
				    3: //12
				      begin
					     if (counter > BLUE_TONE) // > 3 //10
					       begin
						      s = ~s;
						      speaker <= s;
						      counter = 0;
					       end
					     else //11
						   counter = counter + 1;
				      end
				    
				    4: //15
				      begin
					     if (counter > WIN_TONE) // 6 //13
					       begin
						      s = ~s;
						      speaker <= s;
						      counter = 0;
					       end
					     else //14
						   counter = counter + 1;
				      end
				    
				    5: //18
				      begin
					     if (counter > ERROR_TONE) // 1 //16
					       begin
						      s = ~s;
						      speaker <= s;
						      counter = 0;
					       end
					     else //17
						   counter = counter + 1;
				      end
				    
				    default: //19
					  counter = 0;
			      endcase
		       end
		     else //21
		       begin
			      counter = 0;
			      speaker <= 0;
		       end
	      end
     end

   reg [(COD_COLOR-1):0]    count;

   always @ (posedge clock )
     begin
	    if (reset) //23
	      begin
		     count = 0;
		     num <= 0;
	      end
	    else     //24
	      begin
		     // count <= (count + 1) % (2**COD_COLOR) ;
		     // num <= count;
		     // count = (count + 1) % (2**COD_COLOR) ;
			 if (count == 2'b11)
				count = 0;
			 else
			 	count = (count + 1) ;
		     num <= count;
	      end
     end

   reg [(COD_COLOR-1):0]    memory [0:(SIZE_MEM-1)];

   always @ (posedge clock )
     begin
	    if (reset) //25
	      begin
             //integer mar;
		     	data_out <= 0;
		     //for (mar = 0; mar <= SIZE_MEM - 1; mar = mar + 1)
		     	memory[0] = 0;
		     	memory[1] = 0;
				memory[2] = 0;
				memory[3] = 0;
				memory[4] = 0;
				memory[5] = 0;
				memory[6] = 0;
				memory[7] = 0;
				memory[8] = 0;
				memory[9] = 0;
				memory[10] = 0;
				memory[11] = 0;
				memory[12] = 0;
				memory[13] = 0;
				memory[14] = 0;
				memory[15] = 0;
				memory[16] = 0;
				memory[17] = 0;
				memory[18] = 0;
				memory[19] = 0;
				memory[20] = 0;
				memory[21] = 0;
				memory[22] = 0;
				memory[23] = 0;
				memory[24] = 0;
				memory[25] = 0;
				memory[26] = 0;
				memory[27] = 0;
				memory[28] = 0;
				memory[29] = 0;
				memory[30] = 0;
				memory[31] = 0;
//		       memory[mar] <= 0;
	      end
	    else //27
	      begin
		     data_out <= memory[address];
		     if (wr) //26
//			   memory[address] <= data_in;
			   memory[address] = data_in;
	      end
     end

   reg [4:0]                gamma;
   reg [(COD_COLOR-1):0]    ind;
   //reg[((2**COD_COLOR)-1):0] ind;
   reg [(SIZE_ADDRESS-1):0] scan;
   reg [(SIZE_ADDRESS-1):0] max;
   // reg[SIZE_ADDRESS:0] scan;
   // reg[SIZE_ADDRESS:0] max;
   reg [5:0]                timebase;
   reg [5:0]                count2;
   // reg [5:0]                timebase;
   // reg [5:0]                count2;

   always @ (posedge clock )
     begin
	    if (reset) //28
	      begin
		     nloss <= LED_OFF;
		     nl <=  LED_OFF;
		     play <= PLAY_OFF;
		     wr <= 0;
		     scan = 0;
		     max = 0;
		     ind = 0;
		     timebase = 0;
		     count2 = 0;
		     sound <= 0;
		     address <= 0;
		     data_in <= 0;
		     gamma = 0;
	      end
	    else //104
	      begin
		     if (start == 1'b1) //29
			   gamma = 1;
			else 
				gamma = gamma;
		     case (gamma)
		       
			   0: //31
			     begin
				    gamma = 0;
			     end
			   
			   1: // set to zero //32
			     begin
				    nloss <= LED_OFF;
				    nl <= LED_OFF;
				    play <= PLAY_OFF;
				    wr <= 0;
				    max = 0;
				    timebase = COUNT_SEQ; 
				    gamma = 2;
			     end
			   
			   2: //33
			     begin
				    scan = 0;
				    wr <= 1; // begin to write something!!! the num
				    address <= max; // address for data_in
				    data_in <= num; // num to data_in
				    gamma = 3;
			     end
			   
			   3: //34
			     begin
				    wr <= 0; // close the write!!!
				    address <= scan;
				    gamma = 4;
			     end
			   
			   4: //35
			     begin
				    gamma = 5;
			     end
			   
			   5: //36
			     begin
				    nl[data_out] <= LED_ON;
				    count2 = timebase;
				    play <= PLAY_ON;
				    sound <= {1'b0,data_out};
//				    sound <= data_out;
				    gamma = 6;
			     end
			   
			   6: //39
			     begin
				    if (count2 == 0) //37
				      begin
					     nl <= LED_OFF;
					     play <= PLAY_OFF;
					     count2 = timebase;
					     gamma = 7;
				      end
				    else //38
				      begin
					     count2 = count2 - 1;
					     gamma = 6;
				      end
			     end
			   
			   7: //44
			     begin
				    if (count2 == 0) //42
				      begin
					     if (scan != max) //40
					       begin
						      scan = scan + 1;
						      gamma = 3;
					       end
					     else //41
					       begin
						      scan = 0;
						      gamma = 8;
					       end
				      end
				    else //43
				      begin
					     count2 = count2 - 1;
					     gamma = 7;
				      end
			     end
			   
			   8: //45
			     begin
				    count2 = COUNT_KEY;
				    address <= scan;
				    gamma = 9;
			     end
			   
			   9: //46
			     begin
				    gamma = 10;
			     end
			   
			   10: //62
			     begin
				    if (count2 == 0) //47
				      begin
					     nloss <= LED_ON;
					     max = 0;
					     gamma = 17;
				      end
				    else //61
				      begin
					     count2 = count2 - 1;
					     if (k[0] == 1'b1) //50
					       begin
						      ind = 0;
						      sound <= 0;
						      play <= PLAY_ON;
						      count2 = timebase;
						      
						      if (data_out == 0) //48
							    gamma = 11;
						      else //49
						        begin
							       nloss <= LED_ON;
							       gamma = 14;
						        end
					       end
					     else if (k[1] == 1'b1) //60
					       begin
						      ind = 1;
						      sound <= 1;
						      play <= PLAY_ON;
						      count2 = timebase;
						      
						      if (data_out == 1) //51
							    gamma = 11;
						      else //52
						        begin
							       nloss <= LED_ON;
							       gamma = 14;
						        end
					       end
					     else if (k[2] == 1'b1) //59
					       begin
						      ind = 2;
						      sound <= 2;
						      play <= PLAY_ON;
						      count2 = timebase;
						      
						      if (data_out == 2) //53
							    gamma = 11;
						      else //54
						        begin
							       nloss <= LED_ON;
							       gamma = 14;
						        end
					       end
					     else if (k[3] == 1'b1) //58
					       begin
						      ind = 3;
						      sound <= 3;
						      play <= PLAY_ON;
						      count2 = timebase;
						      
						      if (data_out == 3) //55
							    gamma = 11;
						      else //56
						        begin
							       nloss <= LED_ON;
							       gamma = 14;
						        end
					       end
					     else //57
						   gamma = 10;
				      end
			     end
			   
			   11: //63
			     begin
				    nl[ind] <= LED_ON;
				    gamma = 12;
			     end
			   
			   12: //66
			     begin
				    if (count2 == 0) //64
				      begin
					     nl <= LED_OFF;
					     play <= PLAY_OFF;
					     count2 = timebase;
					     gamma = 13;
				      end
				    else //65
				      begin
					     count2 = count2 - 1;
					     gamma = 12;
				      end
			     end
			   
			   13: //72
			     begin
				    if (count2 == 0) //70
				      begin
					     if (scan != max) //67
					       begin
						      scan = scan + 1;
						      gamma = 8;
					       end
					     else if (max != (SIZE_MEM - 1)) //69
					       begin
						      max = max + 1;
						      timebase = timebase - DEC_SEQ;
						      gamma = 2;
					       end
					     else //68
					       begin
						      play <= PLAY_ON;
						      sound <= 4;
						      count2 = COUNT_FIN;
						      gamma = 24;
					       end
				      end
				    else //71
				      begin
					     count2 = count2 - 1;
					     gamma = 13;
				      end
			     end
			   
			   14: //73
			     begin
				    nl[ind] <= LED_ON;
				    gamma = 15;
			     end
			   
			   15: //76
			     begin
				    if (count2 == 0) //74
				      begin
					     nl <= LED_OFF;
					     play <= PLAY_OFF;
					     count2 = timebase;
					     gamma = 16;
				      end
				    else //75
				      begin
					     count2 = count2 - 1;
					     gamma = 15;
				      end
			     end
			   
			   16: //79
			     begin
				    if (count2 == 0) //77
				      begin
					     max = 0;
					     gamma = 17;
				      end
				    else //78
				      begin
					     count2 = count2 - 1;
					     gamma = 16;
				      end
			     end
			   
			   17: //80
			     begin
				    address <= max;
				    gamma = 18;
			     end
			   
			   18: //81
			     begin
				    gamma = 19;
			     end
			   
			   19: //82
			     begin
				    nl[data_out] <= LED_ON;
				    play <= PLAY_ON;
				    sound <= {1'b0,data_out};
//				    sound <= data_out;
				    count2 = timebase;
				    gamma = 20;
			     end
			   
			   20: //85
			     begin
				    if (count2 == 0) //83
				      begin
					     nl <= LED_OFF;
					     play <= PLAY_OFF;
					     count2 = timebase;
					     gamma = 21;
				      end
				    else //84
				      begin
					     count2 = count2 - 1;
					     gamma = 20;
				      end
			     end
			   
			   21: //90
			     begin
				    if (count2 == 0) //88
				      begin
					     if (max != scan) //86
					       begin
						      max = max + 1;
						      gamma = 17;
					       end
					     else //87
					       begin
						      nl[data_out] <= LED_ON;
						      play <= PLAY_ON;
						      sound <= 5;
						      count2 = COUNT_FIN;
						      gamma = 22;
					       end
				      end
				    else //89
				      begin
					     count2 = count2 - 1;
					     gamma = 21;
				      end
			     end
			   
			   22: //93
			     begin
				    if (count2==0) //91
				      begin
					     nl <= LED_OFF;
					     play <= PLAY_OFF;
					     count2 = COUNT_FIN;
					     gamma = 23;
				      end
				    else //92
				      begin
					     count2 = count2 - 1;
					     gamma = 22;
				      end
			     end
			   
			   23: //96
			     begin
				    if (count2==0) //94
				      begin
					     nl[data_out] <= LED_ON;
					     play <= PLAY_ON;
					     sound <= 5;
					     count2 = COUNT_FIN;
					     gamma = 22;
				      end
				    else //95
				      begin
					     count2 = count2 - 1;
					     gamma = 23;
				      end
			     end
			   
			   24: //99
			     begin
				    if (count2==0) //97
				      begin
					     nl <= LED_ON;
					     play <= PLAY_OFF;
					     count2 = COUNT_FIN;
					     gamma = 25;
				      end
				    else //98
				      begin
					     count2 = count2 - 1;
					     gamma = 24;
				      end
			     end
			   
			   25: //102
			     begin
				    if (count2==0) //100
				      begin
					     nl <= LED_OFF;
					     play <= PLAY_ON;
					     sound <= 4;
					     count2 = COUNT_FIN;
					     gamma = 24;
				      end
				    else //101
				      begin
					     count2 = count2 - 1;
					     gamma = 25;
				      end
			     end
			   
			   default: //103
				 gamma = 1;
		     endcase
	      end
     end
endmodule