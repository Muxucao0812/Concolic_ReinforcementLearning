// Following code segment is generated from ./src/b11.v:1
module b11(x_in, stbi, clock, reset, x_out, __obs);
    input [5:0] x_in;
    input stbi;
    input clock;
    input reset;
    output [5:0] x_out;
    input __obs;

    reg [5:0] cont = 6'b0;
    reg signed [8:0] cont1 = 9'b0;
    reg signed [8:0] cont1_inv = 9'b0;
    reg [5:0] r_in = 6'b0;
    reg [3:0] stato = 4'b0;
    reg [5:0] x_out = 6'b0;

    always @(*) begin
        cont1_inv = (9'sb000000000 - cont1); $display(";A 0");		//(assert (= cont1_inv    (bv-sub 0b000000000 cont1 ))) ;0
    end

    // Following code segment is generated from ./src/b11.v:28
    always @(posedge clock) begin
        if ((reset == 1'b1)) begin
            $display("target");
            stato = 4'h0; $display(";A 3");		//(assert (= stato    0h0)) ;3
            r_in = 6'b000000; $display(";A 4");		//(assert (= r_in    0b000000)) ;4
            cont = 6'b000000; $display(";A 5");		//(assert (= cont    0b000000)) ;5
            cont1 = 9'b000000000; $display(";A 6");		//(assert (= cont1    0b000000000)) ;6
            x_out = 6'b000000; $display(";A 7");		//(assert (= x_out    0b000000)) ;7
        end
        else begin
            case (stato)
                4'h0 :
                    begin
                        $display(";A 8");		//(assert (= stato    0h0)) ;8
                        cont = 6'b000000; $display(";A 9");		//(assert (= cont    0b000000)) ;9
                        r_in = x_in; $display(";A 10");		//(assert (= r_in    x_in )) ;10
                        x_out <= #1 6'b000000; $display(";A 11");		//(assert (= x_out    0b000000)) ;11
                        stato = 4'h1; $display(";A 12");		//(assert (= stato    0h1)) ;12
                    end
                4'h1 :
                    begin
                        $display(";A 13");		//(assert (= stato    0h1)) ;13
                        r_in = x_in; $display(";A 14");		//(assert (= r_in    x_in )) ;14
                        if ((stbi == 1'b1)) begin
                            $display(";A 15");		//(assert (= (bv-comp stbi  0b1)   0b1)) ;15
                            stato = 4'h1; $display(";A 17");		//(assert (= stato    0h1)) ;17
                        end
                        else begin
                            $display(";A 16");		//(assert (= (bv-comp stbi  0b1)   0b0)) ;16
                            stato = 4'h2; $display(";A 18");		//(assert (= stato    0h2)) ;18
                        end
                    end
                4'h2 :
                    begin
                        $display(";A 19");		//(assert (= stato    0h2)) ;19
                        if (((r_in == 6'b000000) || (r_in == 6'b111111))) begin
                            $display(";A 20");		//(assert (= (bv-or (bv-comp r_in  0b000000) (bv-comp r_in  0b111111))   0b1)) ;20
                            if ((cont < 6'b011001)) begin
                                $display(";A 22");		//(assert (= (bool-to-bv (bv-lt cont  0b011001))   0b1)) ;22
                                cont = (cont + 6'b000001); $display(";A 24");		//(assert (= cont    (bv-add cont  0b000001))) ;24
                            end
                            else begin
                                $display(";A 23");		//(assert (= (bool-to-bv (bv-lt cont  0b011001))   0b0)) ;23
                                cont = 6'b000000; $display(";A 25");		//(assert (= cont    0b000000)) ;25
                            end
                            cont1 = {3'b000, r_in}; $display(";A 26");		//(assert (= cont1    (bv-concat 0b000 r_in ))) ;26
                            stato = 4'h8; $display(";A 27");		//(assert (= stato    0h8)) ;27
                        end
                        else begin
                            $display(";A 21");		//(assert (= (bv-or (bv-comp r_in  0b000000) (bv-comp r_in  0b111111))   0b0)) ;21
                            if ((r_in <= 6'b011010)) begin
                                $display(";A 28");		//(assert (= (bool-to-bv (bv-le r_in  0b011010))   0b1)) ;28
                                stato = 4'h3; $display(";A 30");		//(assert (= stato    0h3)) ;30
                            end
                            else begin
                                $display(";A 29");		//(assert (= (bool-to-bv (bv-le r_in  0b011010))   0b0)) ;29
                                stato = 4'h1; $display(";A 31");		//(assert (= stato    0h1)) ;31
                            end
                        end
                    end
                4'h3 :
                    begin
                        $display(";A 32");		//(assert (= stato    0h3)) ;32
                        if ((r_in[0] == 1'b1)) begin
                            $display(";A 33");		//(assert (= (bv-comp (bv-extract 0 0 r_in ) 0b1)   0b1)) ;33
                            cont1 = {2'b00, cont, 1'b0}; $display(";A 35");		//(assert (= cont1    (bv-concat 0b00 cont  0b0))) ;35
                        end
                        else begin
                            $display(";A 34");		//(assert (= (bv-comp (bv-extract 0 0 r_in ) 0b1)   0b0)) ;34
                            cont1 = {3'b000, cont}; $display(";A 36");		//(assert (= cont1    (bv-concat 0b000 cont ))) ;36
                        end
                        stato = 4'h4; $display(";A 37");		//(assert (= stato    0h4)) ;37
                    end
                4'h4 :
                    begin
                        $display(";A 38");		//(assert (= stato    0h4)) ;38
                        if ((r_in[1] == 1'b1)) begin
                            $display(";A 39");		//(assert (= (bv-comp (bv-extract 1 1 r_in ) 0b1)   0b1)) ;39
                            cont1 = ({3'b000, r_in} + cont1); $display(";A 41");		//(assert (= cont1    (bv-add (bv-concat 0b000 r_in ) cont1 ))) ;41
                            stato = 4'h5; $display(";A 42");		//(assert (= stato    0h5)) ;42
                        end
                        else begin
                            $display(";A 40");		//(assert (= (bv-comp (bv-extract 1 1 r_in ) 0b1)   0b0)) ;40
                            cont1 = ({3'b000, r_in} - cont1); $display(";A 43");		//(assert (= cont1    (bv-sub (bv-concat 0b000 r_in ) cont1 ))) ;43
                            stato = 4'h6; $display(";A 44");		//(assert (= stato    0h6)) ;44
                        end
                    end
                4'h5 :
                    begin
                        $display(";A 45");		//(assert (= stato    0h5)) ;45
                        if ((cont1 > 9'sb000011010)) begin
                            $display(";A 46");		//(assert (= (bool-to-bv (bv-sgt cont1  0b000011010))   0b1)) ;46
                            cont1 = (cont1 - 9'b000011010); $display(";A 48");		//(assert (= cont1    (bv-sub cont1  0b000011010))) ;48
                            stato = 4'h5; $display(";A 49");		//(assert (= stato    0h5)) ;49
                        end
                        else begin
                            $display(";A 47");		//(assert (= (bool-to-bv (bv-sgt cont1  0b000011010))   0b0)) ;47
                            stato = 4'h7; $display(";A 50");		//(assert (= stato    0h7)) ;50
                        end
                    end
                4'h6 :
                    begin
                        $display(";A 51");		//(assert (= stato    0h6)) ;51
                        if ((cont1 > 9'sb000111111)) begin
                            $display(";A 52");		//(assert (= (bool-to-bv (bv-sgt cont1  0b000111111))   0b1)) ;52
                            cont1 = (cont1 + 9'b000011010); $display(";A 54");		//(assert (= cont1    (bv-add cont1  0b000011010))) ;54
                            stato = 4'h6; $display(";A 55");		//(assert (= stato    0h6)) ;55
                        end
                        else begin
                            $display(";A 53");		//(assert (= (bool-to-bv (bv-sgt cont1  0b000111111))   0b0)) ;53
                            stato = 4'h7; $display(";A 56");		//(assert (= stato    0h7)) ;56
                        end
                    end
                4'h7 :
                    begin
                        $display(";A 57");		//(assert (= stato    0h7)) ;57
                        if ((r_in[3:2] == 2'b00)) begin
                            $display(";A 58");		//(assert (= (bv-comp (bv-extract 3 2 r_in ) 0b00)   0b1)) ;58
                            cont1 = (cont1 - 9'b000010101); $display(";A 60");		//(assert (= cont1    (bv-sub cont1  0b000010101))) ;60
                        end
                        else begin
                            $display(";A 59");		//(assert (= (bv-comp (bv-extract 3 2 r_in ) 0b00)   0b0)) ;59
                            if ((r_in[3:2] == 2'b01)) begin
                                $display(";A 61");		//(assert (= (bv-comp (bv-extract 3 2 r_in ) 0b01)   0b1)) ;61
                                cont1 = (cont1 - 9'b000101010); $display(";A 63");		//(assert (= cont1    (bv-sub cont1  0b000101010))) ;63
                            end
                            else begin
                                $display(";A 62");		//(assert (= (bv-comp (bv-extract 3 2 r_in ) 0b01)   0b0)) ;62
                                if ((r_in[3:2] == 2'b10)) begin
                                    $display(";A 64");		//(assert (= (bv-comp (bv-extract 3 2 r_in ) 0b10)   0b1)) ;64
                                    cont1 = (cont1 + 9'b000000111); $display(";A 66");		//(assert (= cont1    (bv-add cont1  0b000000111))) ;66
                                end
                                else begin
                                    $display(";A 65");		//(assert (= (bv-comp (bv-extract 3 2 r_in ) 0b10)   0b0)) ;65
                                    cont1 = (cont1 + 9'b000011100); $display(";A 67");		//(assert (= cont1    (bv-add cont1  0b000011100))) ;67
                                end
                            end
                        end
                        stato = 4'h8; $display(";A 68");		//(assert (= stato    0h8)) ;68
                    end
                4'h8 :
                    begin
                        $display(";A 69");		//(assert (= stato    0h8)) ;69
                        if ((cont1 < 9'sb000000000)) begin
                            $display(";A 70");		//(assert (= (bool-to-bv (bv-slt cont1  0b000000000))   0b1)) ;70
                            x_out <= #1 cont1_inv[5:0]; $display(";A 72");		//(assert (= x_out    (bv-extract 5 0 cont1_inv ))) ;72
                        end
                        else begin
                            $display(";A 71");		//(assert (= (bool-to-bv (bv-slt cont1  0b000000000))   0b0)) ;71
                            x_out <= #1 cont1[5:0]; $display(";A 73");		//(assert (= x_out    (bv-extract 5 0 cont1 ))) ;73
                        end
                        stato = 4'h1; $display(";A 74");		//(assert (= stato    0h1)) ;74
                    end
            endcase
        end
    end

endmodule

