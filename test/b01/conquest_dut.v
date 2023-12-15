// Following code segment is generated from ./src/b01.v:1
module b01(clock, line1, line2, reset, outp, overflw, __obs);
    input clock;
    input line1;
    input line2;
    input reset;
    output outp;
    output overflw;
    input __obs;

    reg outp = 1'b0;
    reg overflw = 1'b0;
    reg [2:0] stato = 3'b0;

    // Following code segment is generated from ./src/b01.v:23
    always @(posedge clock) begin
        if ((reset == 1'b1)) begin
            stato <= #1 3'b000; $display(";A 2");		//(assert (= stato    0b000)) ;2
            outp <= #1 1'b0; $display(";A 3");		//(assert (= outp    0b0)) ;3
            overflw <= #1 1'b0; $display(";A 4");		//(assert (= overflw    0b0)) ;4
        end
        else begin
            case (stato)
                3'b000 :
                    begin
                        $display(";A 5");		//(assert (= stato    0b000)) ;5
                        if (((line1 == 1'b1) && (line2 == 1'b1))) begin
                            $display(";A 6");		//(assert (= (bv-and (bv-comp line1  0b1) (bv-comp line2  0b1))   0b1)) ;6
                            stato <= #1 3'b100; $display(";A 8");		//(assert (= stato    0b100)) ;8
                        end
                        else begin
                            $display(";A 7");		//(assert (= (bv-and (bv-comp line1  0b1) (bv-comp line2  0b1))   0b0)) ;7
                            stato <= #1 3'b001; $display(";A 9");		//(assert (= stato    0b001)) ;9
                        end
                        outp <= #1 (line1 | line2); $display(";A 10");		//(assert (= outp    (bv-or line1  line2 ))) ;10
                        overflw <= #1 1'b0; $display(";A 11");		//(assert (= overflw    0b0)) ;11
                    end
                3'b011 :
                    begin
                        $display(";A 12");		//(assert (= stato    0b011)) ;12
                        if (((line1 == 1'b1) && (line2 == 1'b1))) begin
                            $display(";A 13");		//(assert (= (bv-and (bv-comp line1  0b1) (bv-comp line2  0b1))   0b1)) ;13
                            stato <= #1 3'b100; $display(";A 15");		//(assert (= stato    0b100)) ;15
                        end
                        else begin
                            $display(";A 14");		//(assert (= (bv-and (bv-comp line1  0b1) (bv-comp line2  0b1))   0b0)) ;14
                            stato <= #1 3'b001; $display(";A 16");		//(assert (= stato    0b001)) ;16
                        end
                        outp <= #1 (line1 | line2); $display(";A 17");		//(assert (= outp    (bv-or line1  line2 ))) ;17
                        overflw <= #1 1'b1; $display(";A 18");		//(assert (= overflw    0b1)) ;18
                    end
                3'b001 :
                    begin
                        $display(";A 19");		//(assert (= stato    0b001)) ;19
                        if (((line1 == 1'b1) && (line2 == 1'b1))) begin
                            $display(";A 20");		//(assert (= (bv-and (bv-comp line1  0b1) (bv-comp line2  0b1))   0b1)) ;20
                            stato <= #1 3'b101; $display(";A 22");		//(assert (= stato    0b101)) ;22
                        end
                        else begin
                            $display(";A 21");		//(assert (= (bv-and (bv-comp line1  0b1) (bv-comp line2  0b1))   0b0)) ;21
                            stato <= #1 3'b010; $display(";A 23");		//(assert (= stato    0b010)) ;23
                        end
                        outp <= #1 (line1 | line2); $display(";A 24");		//(assert (= outp    (bv-or line1  line2 ))) ;24
                        overflw <= #1 1'b0; $display(";A 25");		//(assert (= overflw    0b0)) ;25
                    end
                3'b100 :
                    begin
                        $display(";A 26");		//(assert (= stato    0b100)) ;26
                        if (((line1 == 1'b1) || (line2 == 1'b1))) begin
                            $display(";A 27");		//(assert (= (bv-or (bv-comp line1  0b1) (bv-comp line2  0b1))   0b1)) ;27
                            stato <= #1 3'b101; $display(";A 29");		//(assert (= stato    0b101)) ;29
                        end
                        else begin
                            $display(";A 28");		//(assert (= (bv-or (bv-comp line1  0b1) (bv-comp line2  0b1))   0b0)) ;28
                            stato <= #1 3'b010; $display(";A 30");		//(assert (= stato    0b010)) ;30
                        end
                        outp <= #1 (~(line1 ^ line2)); $display(";A 31");		//(assert (= outp    (bv-not (bv-xor line1  line2 )))) ;31
                        overflw <= #1 1'b0; $display(";A 32");		//(assert (= overflw    0b0)) ;32
                    end
                3'b010 :
                    begin
                        $display(";A 33");		//(assert (= stato    0b010)) ;33
                        if (((line1 == 1'b1) && (line2 == 1'b1))) begin
                            $display(";A 34");		//(assert (= (bv-and (bv-comp line1  0b1) (bv-comp line2  0b1))   0b1)) ;34
                            stato <= #1 3'b111; $display(";A 36");		//(assert (= stato    0b111)) ;36
                        end
                        else begin
                            $display(";A 35");		//(assert (= (bv-and (bv-comp line1  0b1) (bv-comp line2  0b1))   0b0)) ;35
                            stato <= #1 3'b110; $display(";A 37");		//(assert (= stato    0b110)) ;37
                        end
                        outp <= #1 (line1 | line2); $display(";A 38");		//(assert (= outp    (bv-or line1  line2 ))) ;38
                        overflw <= #1 1'b0; $display(";A 39");		//(assert (= overflw    0b0)) ;39
                    end
                3'b101 :
                    begin
                        $display(";A 40");		//(assert (= stato    0b101)) ;40
                        if (((line1 == 1'b1) || (line2 == 1'b1))) begin
                            $display(";A 41");		//(assert (= (bv-or (bv-comp line1  0b1) (bv-comp line2  0b1))   0b1)) ;41
                            stato <= #1 3'b111; $display(";A 43");		//(assert (= stato    0b111)) ;43
                        end
                        else begin
                            $display(";A 42");		//(assert (= (bv-or (bv-comp line1  0b1) (bv-comp line2  0b1))   0b0)) ;42
                            stato <= #1 3'b110; $display(";A 44");		//(assert (= stato    0b110)) ;44
                        end
                        outp <= #1 (~(line1 | line2)); $display(";A 45");		//(assert (= outp    (bv-not (bv-or line1  line2 )))) ;45
                        overflw <= #1 1'b0; $display(";A 46");		//(assert (= overflw    0b0)) ;46
                    end
                3'b110 :
                    begin
                        $display(";A 47");		//(assert (= stato    0b110)) ;47
                        if (((line1 == 1'b1) && (line2 == 1'b1))) begin
                            $display(";A 48");		//(assert (= (bv-and (bv-comp line1  0b1) (bv-comp line2  0b1))   0b1)) ;48
                            stato <= #1 3'b011; $display(";A 50");		//(assert (= stato    0b011)) ;50
                        end
                        else begin
                            $display(";A 49");		//(assert (= (bv-and (bv-comp line1  0b1) (bv-comp line2  0b1))   0b0)) ;49
                            stato <= #1 3'b000; $display(";A 51");		//(assert (= stato    0b000)) ;51
                        end
                        outp <= #1 (line1 | line2); $display(";A 52");		//(assert (= outp    (bv-or line1  line2 ))) ;52
                        overflw <= #1 1'b0; $display(";A 53");		//(assert (= overflw    0b0)) ;53
                    end
                3'b111 :
                    begin
                        $display(";A 54");		//(assert (= stato    0b111)) ;54
                        if (((line1 == 1'b1) || (line2 == 1'b1))) begin
                            $display(";A 55");		//(assert (= (bv-or (bv-comp line1  0b1) (bv-comp line2  0b1))   0b1)) ;55
                            stato <= #1 3'b011; $display(";A 57");		//(assert (= stato    0b011)) ;57
                        end
                        else begin
                            $display(";A 56");		//(assert (= (bv-or (bv-comp line1  0b1) (bv-comp line2  0b1))   0b0)) ;56
                            stato <= #1 3'b000; $display(";A 58");		//(assert (= stato    0b000)) ;58
                        end
                        outp <= #1 (~(line1 | line2)); $display(";A 59");		//(assert (= outp    (bv-not (bv-or line1  line2 )))) ;59
                        overflw <= #1 1'b0; $display(";A 60");		//(assert (= overflw    0b0)) ;60
                    end
            endcase
        end
    end

endmodule

