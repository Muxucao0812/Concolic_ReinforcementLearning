// Following code segment is generated from /home/meng/Code/concolic-testing/test/b06/src/b06.v:17
module b06(eql, clock, reset, cont_eql, cc_mux, uscite, enable_count, ackout, __obs);
    input eql;
    input clock;
    input reset;
    input cont_eql;
    output [1:0] cc_mux;
    output [1:0] uscite;
    output enable_count;
    output ackout;
    input __obs;

    reg ackout = 1'b0;
    reg [1:0] cc_mux = 2'b0;
    reg enable_count = 1'b0;
    reg [2:0] state = 3'b0;
    reg [1:0] uscite = 2'b0;

    // Following code segment is generated from /home/meng/Code/concolic-testing/test/b06/src/b06.v:47
    always @(posedge clock) begin
        if ((reset == 1'b1)) begin
            state <= #1 3'b000; $display(";A 2");		//(assert (= state    0b000)) ;2
            cc_mux <= #1 2'b00; $display(";A 3");		//(assert (= cc_mux    0b00)) ;3
            enable_count <= #1 1'b0; $display(";A 4");		//(assert (= enable_count    0b0)) ;4
            ackout <= #1 1'b0; $display(";A 5");		//(assert (= ackout    0b0)) ;5
            uscite <= #1 2'b00; $display(";A 6");		//(assert (= uscite    0b00)) ;6
        end
        else begin
            if ((cont_eql == 1'b1)) begin
                $display(";A 7");		//(assert (= (bv-comp cont_eql  0b1)   0b1)) ;7
                ackout <= #1 1'b0; $display(";A 9");		//(assert (= ackout    0b0)) ;9
                enable_count <= #1 1'b0; $display(";A 10");		//(assert (= enable_count    0b0)) ;10
            end
            else begin
                $display(";A 8");		//(assert (= (bv-comp cont_eql  0b1)   0b0)) ;8
                ackout <= #1 1'b1; $display(";A 11");		//(assert (= ackout    0b1)) ;11
                enable_count <= #1 1'b1; $display(";A 12");		//(assert (= enable_count    0b1)) ;12
            end
            case (state)
                3'b000 :
                    begin
                        $display(";A 13");		//(assert (= state    0b000)) ;13
                        cc_mux <= #1 2'b01; $display(";A 14");		//(assert (= cc_mux    0b01)) ;14
                        uscite <= #1 2'b01; $display(";A 15");		//(assert (= uscite    0b01)) ;15
                        state <= #1 3'b001; $display(";A 16");		//(assert (= state    0b001)) ;16
                    end
                3'b001 :
                    begin
                        $display(";A 17");		//(assert (= state    0b001)) ;17
                        if ((eql == 1'b1)) begin
                            $display(";A 18");		//(assert (= (bv-comp eql  0b1)   0b1)) ;18
                            uscite <= #1 2'b00; $display(";A 20");		//(assert (= uscite    0b00)) ;20
                            cc_mux <= #1 2'b11; $display(";A 21");		//(assert (= cc_mux    0b11)) ;21
                            state <= #1 3'b010; $display(";A 22");		//(assert (= state    0b010)) ;22
                        end
                        else begin
                            $display(";A 19");		//(assert (= (bv-comp eql  0b1)   0b0)) ;19
                            uscite <= #1 2'b01; $display(";A 23");		//(assert (= uscite    0b01)) ;23
                            cc_mux <= #1 2'b10; $display(";A 24");		//(assert (= cc_mux    0b10)) ;24
                            state <= #1 3'b101; $display(";A 25");		//(assert (= state    0b101)) ;25
                        end
                    end
                3'b101 :
                    begin
                        $display(";A 26");		//(assert (= state    0b101)) ;26
                        if ((eql == 1'b1)) begin
                            $display(";A 27");		//(assert (= (bv-comp eql  0b1)   0b1)) ;27
                            uscite <= #1 2'b00; $display(";A 29");		//(assert (= uscite    0b00)) ;29
                            cc_mux <= #1 2'b11; $display(";A 30");		//(assert (= cc_mux    0b11)) ;30
                            state <= #1 3'b100; $display(";A 31");		//(assert (= state    0b100)) ;31
                        end
                        else begin
                            $display(";A 28");		//(assert (= (bv-comp eql  0b1)   0b0)) ;28
                            uscite <= #1 2'b01; $display(";A 32");		//(assert (= uscite    0b01)) ;32
                            cc_mux <= #1 2'b01; $display(";A 33");		//(assert (= cc_mux    0b01)) ;33
                            state <= #1 3'b001; $display(";A 34");		//(assert (= state    0b001)) ;34
                        end
                    end
                3'b010 :
                    begin
                        $display(";A 35");		//(assert (= state    0b010)) ;35
                        if ((eql == 1'b1)) begin
                            $display(";A 36");		//(assert (= (bv-comp eql  0b1)   0b1)) ;36
                            uscite <= #1 2'b00; $display(";A 38");		//(assert (= uscite    0b00)) ;38
                            cc_mux <= #1 2'b11; $display(";A 39");		//(assert (= cc_mux    0b11)) ;39
                            state <= #1 3'b010; $display(";A 40");		//(assert (= state    0b010)) ;40
                        end
                        else begin
                            $display(";A 37");		//(assert (= (bv-comp eql  0b1)   0b0)) ;37
                            uscite <= #1 2'b01; $display(";A 41");		//(assert (= uscite    0b01)) ;41
                            ackout <= #1 1'b1; $display(";A 42");		//(assert (= ackout    0b1)) ;42
                            enable_count <= #1 1'b1; $display(";A 43");		//(assert (= enable_count    0b1)) ;43
                            cc_mux <= #1 2'b01; $display(";A 44");		//(assert (= cc_mux    0b01)) ;44
                            state <= #1 3'b011; $display(";A 45");		//(assert (= state    0b011)) ;45
                        end
                    end
                3'b011 :
                    begin
                        $display(";A 46");		//(assert (= state    0b011)) ;46
                        if ((eql == 1'b1)) begin
                            $display(";A 47");		//(assert (= (bv-comp eql  0b1)   0b1)) ;47
                            uscite <= #1 2'b01; $display(";A 49");		//(assert (= uscite    0b01)) ;49
                            cc_mux <= #1 2'b01; $display(";A 50");		//(assert (= cc_mux    0b01)) ;50
                            state <= #1 3'b011; $display(";A 51");		//(assert (= state    0b011)) ;51
                        end
                        else begin
                            $display(";A 48");		//(assert (= (bv-comp eql  0b1)   0b0)) ;48
                            uscite <= #1 2'b01; $display(";A 52");		//(assert (= uscite    0b01)) ;52
                            cc_mux <= #1 2'b01; $display(";A 53");		//(assert (= cc_mux    0b01)) ;53
                            state <= #1 3'b001; $display(";A 54");		//(assert (= state    0b001)) ;54
                        end
                    end
                3'b100 :
                    begin
                        $display(";A 55");		//(assert (= state    0b100)) ;55
                        if ((eql == 1'b1)) begin
                            $display(";A 56");		//(assert (= (bv-comp eql  0b1)   0b1)) ;56
                            uscite <= #1 2'b00; $display(";A 58");		//(assert (= uscite    0b00)) ;58
                            cc_mux <= #1 2'b11; $display(";A 59");		//(assert (= cc_mux    0b11)) ;59
                            state <= #1 3'b100; $display(";A 60");		//(assert (= state    0b100)) ;60
                        end
                        else begin
                            $display(";A 57");		//(assert (= (bv-comp eql  0b1)   0b0)) ;57
                            uscite <= #1 2'b11; $display(";A 61");		//(assert (= uscite    0b11)) ;61
                            cc_mux <= #1 2'b10; $display(";A 62");		//(assert (= cc_mux    0b10)) ;62
                            state <= #1 3'b110; $display(";A 63");		//(assert (= state    0b110)) ;63
                        end
                    end
                3'b110 :
                    begin
                        $display(";A 64");		//(assert (= state    0b110)) ;64
                        if ((eql == 1'b1)) begin
                            $display(";A 65");		//(assert (= (bv-comp eql  0b1)   0b1)) ;65
                            uscite <= #1 2'b11; $display(";A 67");		//(assert (= uscite    0b11)) ;67
                            cc_mux <= #1 2'b10; $display(";A 68");		//(assert (= cc_mux    0b10)) ;68
                            state <= #1 3'b110; $display(";A 69");		//(assert (= state    0b110)) ;69
                        end
                        else begin
                            $display(";A 66");		//(assert (= (bv-comp eql  0b1)   0b0)) ;66
                            uscite <= #1 2'b01; $display(";A 70");		//(assert (= uscite    0b01)) ;70
                            cc_mux <= #1 2'b01; $display(";A 71");		//(assert (= cc_mux    0b01)) ;71
                            state <= #1 3'b001; $display(";A 72");		//(assert (= state    0b001)) ;72
                        end
                    end
            endcase
        end
    end

    // Displaying module variables
    always @(posedge clock) begin
      $display("Reg: ackout = %b", ackout);
      $display("Reg: cc_mux = %b", cc_mux);
      $display("Reg: enable_count = %b", enable_count);
      $display("Reg: state = %b", state);
      $display("Reg: uscite = %b", uscite);
    end
endmodule

