// Following code segment is generated from /home/meng/Code/concolic-testing/test/grammerTest/src/grammerTest.v:1
module grammerTest(clk, reset, in, out, sig_display, __obs);
    input clk;
    input reset;
    input [31:0] in;
    output [31:0] out;
    input sig_display;
    input __obs;

    reg [1:0] addr;
    reg [7:0] cnt;
    reg [31:0] myArray [0:3];
    reg [31:0] out;
    reg [31:0] temp;

    // Following code segment is generated from /home/meng/Code/concolic-testing/test/grammerTest/src/grammerTest.v:14
    always @(posedge clk or negedge reset) begin
        if ((reset == 1'b1)) begin
            addr <= #1 2'b00; $display(";A 2");		//(assert (= addr    0b00)) ;2
            temp <= #1 32'h00000000; $display(";A 3");		//(assert (= temp    0h00000000)) ;3
            cnt <= #1 8'h00; $display(";A 4");		//(assert (= cnt    0h00)) ;4
        end
        else begin
            addr <= #1 (addr + 2'b01); $display(";A 5");		//(assert (= addr    (bv-add addr  0b01))) ;5
            temp <= #1 in; $display(";A 6");		//(assert (= temp    in )) ;6
            cnt <= #1 (cnt + 8'h01); $display(";A 7");		//(assert (= cnt    (bv-add cnt  0h01))) ;7
        end
    end

    // Following code segment is generated from /home/meng/Code/concolic-testing/test/grammerTest/src/grammerTest.v:27
    always @(posedge clk) begin
        if ((cnt == 8'h00)) begin
            $display(";A 8");		//(assert (= (bv-comp cnt  0h00)   0b1)) ;8
            case (addr)
                2'b00 :
                    begin
                        $display(";A 10");		//(assert (= addr    0b00)) ;10
                        myArray[6'b000000] <= #1 temp; $display(";A 11");		//(assert (= myArray 0    temp )) ;11
                    end
                2'b01 :
                    begin
                        $display(";A 12");		//(assert (= addr    0b01)) ;12
                        myArray[6'b000001] <= #1 temp; $display(";A 13");		//(assert (= myArray 1    temp )) ;13
                    end
                2'b10 :
                    begin
                        $display(";A 14");		//(assert (= addr    0b10)) ;14
                        myArray[6'b000010] <= #1 temp; $display(";A 15");		//(assert (= myArray 2    temp )) ;15
                    end
                2'b11 :
                    begin
                        $display(";A 16");		//(assert (= addr    0b11)) ;16
                        myArray[6'b000011] <= #1 temp; $display(";A 17");		//(assert (= myArray 3    temp )) ;17
                    end
            endcase
        end
        else begin
            $display(";A 9");		//(assert (= (bv-comp cnt  0h00)   0b0)) ;9
            if ((cnt < 8'h80)) begin
                $display(";A 18");		//(assert (= (bool-to-bv (bv-lt cnt  0h80))   0b1)) ;18
                case (addr)
                    2'b00 :
                        begin
                            $display(";A 20");		//(assert (= addr    0b00)) ;20
                            myArray[6'b000000] <= #1 (temp / 32'h00000002); $display(";A 21");		//(assert (= myArray 0    (bv-div temp  0h00000002))) ;21
                        end
                    2'b01 :
                        begin
                            $display(";A 22");		//(assert (= addr    0b01)) ;22
                            myArray[6'b000001] <= #1 (temp / 32'h00000002); $display(";A 23");		//(assert (= myArray 1    (bv-div temp  0h00000002))) ;23
                        end
                    2'b10 :
                        begin
                            $display(";A 24");		//(assert (= addr    0b10)) ;24
                            myArray[6'b000010] <= #1 (temp / 32'h00000002); $display(";A 25");		//(assert (= myArray 2    (bv-div temp  0h00000002))) ;25
                        end
                    2'b11 :
                        begin
                            $display(";A 26");		//(assert (= addr    0b11)) ;26
                            myArray[6'b000011] <= #1 (temp / 32'h00000002); $display(";A 27");		//(assert (= myArray 3    (bv-div temp  0h00000002))) ;27
                        end
                endcase
            end
            else begin
                $display(";A 19");		//(assert (= (bool-to-bv (bv-lt cnt  0h80))   0b0)) ;19
                if ((cnt < 8'hC0)) begin
                    $display(";A 28");		//(assert (= (bool-to-bv (bv-lt cnt  0hC0))   0b1)) ;28
                    case (addr)
                        2'b00 :
                            begin
                                $display(";A 30");		//(assert (= addr    0b00)) ;30
                                myArray[6'b000000] <= #1 (temp >> 32'h00000002); $display(";A 31");		//(assert (= myArray 0    (bv-lshr temp  0h00000002))) ;31
                            end
                        2'b01 :
                            begin
                                $display(";A 32");		//(assert (= addr    0b01)) ;32
                                myArray[6'b000001] <= #1 (temp >> 32'h00000002); $display(";A 33");		//(assert (= myArray 1    (bv-lshr temp  0h00000002))) ;33
                            end
                        2'b10 :
                            begin
                                $display(";A 34");		//(assert (= addr    0b10)) ;34
                                myArray[6'b000010] <= #1 (temp >> 32'h00000002); $display(";A 35");		//(assert (= myArray 2    (bv-lshr temp  0h00000002))) ;35
                            end
                        2'b11 :
                            begin
                                $display(";A 36");		//(assert (= addr    0b11)) ;36
                                myArray[6'b000011] <= #1 (temp >> 32'h00000002); $display(";A 37");		//(assert (= myArray 3    (bv-lshr temp  0h00000002))) ;37
                            end
                    endcase
                end
                else begin
                    $display(";A 29");		//(assert (= (bool-to-bv (bv-lt cnt  0hC0))   0b0)) ;29
                    case (addr)
                        2'b00 :
                            begin
                                $display(";A 38");		//(assert (= addr    0b00)) ;38
                                myArray[6'b000000] <= #1 32'h00000000; $display(";A 39");		//(assert (= myArray 0    0h00000000)) ;39
                            end
                        2'b01 :
                            begin
                                $display(";A 40");		//(assert (= addr    0b01)) ;40
                                myArray[6'b000001] <= #1 32'h00000000; $display(";A 41");		//(assert (= myArray 1    0h00000000)) ;41
                            end
                        2'b10 :
                            begin
                                $display(";A 42");		//(assert (= addr    0b10)) ;42
                                myArray[6'b000010] <= #1 32'h00000000; $display(";A 43");		//(assert (= myArray 2    0h00000000)) ;43
                            end
                        2'b11 :
                            begin
                                $display(";A 44");		//(assert (= addr    0b11)) ;44
                                myArray[6'b000011] <= #1 32'h00000000; $display(";A 45");		//(assert (= myArray 3    0h00000000)) ;45
                            end
                    endcase
                end
            end
        end
        out <= #1 myArray[addr]; $display(";A 46");		//(assert (= out    ( myArray addr ))) ;46
    end

endmodule

