// Following code segment is generated from ./src/grammerTest.v:1
module grammerTest(clk, reset, in, count1, count, register, out, out1, __obs);
    input clk;
    input reset;
    input [31:0] in;
    input [3:0] count1;
    input [3:0] count;
    input register;
    output [31:0] out;
    output [31:0] out1;
    input __obs;

    reg [1:0] addr = 2'b0;
    reg [7:0] cnt = 8'b0;
    reg [31:0] myArray [0:3];
    reg [31:0] out = 32'b0;
    reg [31:0] out1 = 32'b0;
    reg [31:0] temp = 32'b0;
    reg [31:0] temp1 = 32'b0;

    // Following code segment is generated from ./src/grammerTest.v:19
    always @(posedge clk) begin
        if ((reset == 1'b1)) begin
            temp1 <= #1 32'h00000000; $display(";A 2");		//(assert (= temp1    0h00000000)) ;2
        end
        else begin
        end
        // Displaying module variables
        begin
            $display(";R addr = %b", addr);
            $display(";R cnt = %b", cnt);
            $display(";R myArray[0] = %b", myArray[0]);
            $display(";R myArray[1] = %b", myArray[1]);
            $display(";R myArray[2] = %b", myArray[2]);
            $display(";R myArray[3] = %b", myArray[3]);
            $display(";R out = %b", out);
            $display(";R out1 = %b", out1);
            $display(";R temp = %b", temp);
            $display(";R temp1 = %b", temp1);
        end
    end

    // Following code segment is generated from ./src/grammerTest.v:25
    always @(posedge clk) begin
        if ((reset == 1'b1)) begin
            addr <= #1 2'b00; $display(";A 5");		//(assert (= addr    0b00)) ;5
            temp <= #1 32'h00000000; $display(";A 6");		//(assert (= temp    0h00000000)) ;6
            cnt <= #1 8'h00; $display(";A 7");		//(assert (= cnt    0h00)) ;7
        end
        else begin
            addr <= #1 (addr + 2'b01); $display(";A 8");		//(assert (= addr    (bv-add addr  0b01))) ;8
            temp <= #1 in; $display(";A 9");		//(assert (= temp    in )) ;9
            cnt <= #1 (cnt + 8'h01); $display(";A 10");		//(assert (= cnt    (bv-add cnt  0h01))) ;10
        end
        // Displaying module variables
        begin
            $display(";R addr = %b", addr);
            $display(";R cnt = %b", cnt);
            $display(";R myArray[0] = %b", myArray[0]);
            $display(";R myArray[1] = %b", myArray[1]);
            $display(";R myArray[2] = %b", myArray[2]);
            $display(";R myArray[3] = %b", myArray[3]);
            $display(";R out = %b", out);
            $display(";R out1 = %b", out1);
            $display(";R temp = %b", temp);
            $display(";R temp1 = %b", temp1);
        end
    end

    // Following code segment is generated from ./src/grammerTest.v:38
    always @(posedge clk) begin
        if ((!cnt)) begin
            $display(";A 11");		//(assert (= (bv-not (bv-redor cnt ))   0b1)) ;11
            case (addr)
                2'b00 :
                    begin
                        $display(";A 13");		//(assert (= addr    0b00)) ;13
                        myArray[2'b00] <= #1 (temp % 32'h00000005); $display(";A 14");		//(assert (= myArray 0    (bv-rem temp  0h00000005))) ;14
                        addr <= #1 (addr + 2'b01); $display(";A 15");		//(assert (= addr    (bv-add addr  0b01))) ;15
                    end
                2'b01 :
                    begin
                        $display(";A 16");		//(assert (= addr    0b01)) ;16
                        myArray[2'b01] <= #1 (temp ** 32'h00000005); $display(";A 17");		//(assert (= myArray 1    (bv-pow temp  0h00000005))) ;17
                    end
                2'b10 :
                    begin
                        $display(";A 18");		//(assert (= addr    0b10)) ;18
                        myArray[2'b10] <= #1 (temp ** 32'h00000002); $display(";A 19");		//(assert (= myArray 2    (bv-pow temp  0h00000002))) ;19
                    end
                2'b11 :
                    begin
                        $display(";A 20");		//(assert (= addr    0b11)) ;20
                        myArray[2'b11] <= #1 (temp % 32'h00000005); $display(";A 21");		//(assert (= myArray 3    (bv-rem temp  0h00000005))) ;21
                    end
            endcase
        end
        else begin
            $display(";A 12");		//(assert (= (bv-not (bv-redor cnt ))   0b0)) ;12
            if ((cnt < 8'h80)) begin
                $display(";A 22");		//(assert (= (bool-to-bv (bv-lt cnt  0h80))   0b1)) ;22
                case (addr)
                    2'b00 :
                        begin
                            $display(";A 24");		//(assert (= addr    0b00)) ;24
                            myArray[2'b00] <= #1 (temp / 32'h00000002); $display(";A 25");		//(assert (= myArray 0    (bv-div temp  0h00000002))) ;25
                        end
                    2'b01 :
                        begin
                            $display(";A 26");		//(assert (= addr    0b01)) ;26
                            myArray[2'b01] <= #1 (temp / 32'h00000002); $display(";A 27");		//(assert (= myArray 1    (bv-div temp  0h00000002))) ;27
                        end
                    2'b10 :
                        begin
                            $display(";A 28");		//(assert (= addr    0b10)) ;28
                            myArray[2'b10] <= #1 (temp / 32'h00000002); $display(";A 29");		//(assert (= myArray 2    (bv-div temp  0h00000002))) ;29
                        end
                    2'b11 :
                        begin
                            $display(";A 30");		//(assert (= addr    0b11)) ;30
                            myArray[2'b11] <= #1 (temp / 32'h00000002); $display(";A 31");		//(assert (= myArray 3    (bv-div temp  0h00000002))) ;31
                        end
                endcase
            end
            else begin
                $display(";A 23");		//(assert (= (bool-to-bv (bv-lt cnt  0h80))   0b0)) ;23
                if ((cnt < 8'hC0)) begin
                    $display(";A 32");		//(assert (= (bool-to-bv (bv-lt cnt  0hC0))   0b1)) ;32
                    case (addr)
                        2'b00 :
                            begin
                                $display(";A 34");		//(assert (= addr    0b00)) ;34
                                myArray[2'b00] <= #1 (temp >> 32'h00000002); $display(";A 35");		//(assert (= myArray 0    (bv-lshr temp  0h00000002))) ;35
                            end
                        2'b01 :
                            begin
                                $display(";A 36");		//(assert (= addr    0b01)) ;36
                                myArray[2'b01] <= #1 (temp >> 32'h00000002); $display(";A 37");		//(assert (= myArray 1    (bv-lshr temp  0h00000002))) ;37
                            end
                        2'b10 :
                            begin
                                $display(";A 38");		//(assert (= addr    0b10)) ;38
                                myArray[2'b10] <= #1 (temp >> 32'h00000002); $display(";A 39");		//(assert (= myArray 2    (bv-lshr temp  0h00000002))) ;39
                            end
                        2'b11 :
                            begin
                                $display(";A 40");		//(assert (= addr    0b11)) ;40
                                myArray[2'b11] <= #1 (temp >> 32'h00000002); $display(";A 41");		//(assert (= myArray 3    (bv-lshr temp  0h00000002))) ;41
                            end
                    endcase
                end
                else begin
                    $display(";A 33");		//(assert (= (bool-to-bv (bv-lt cnt  0hC0))   0b0)) ;33
                    case (addr)
                        2'b00 :
                            begin
                                $display(";A 42");		//(assert (= addr    0b00)) ;42
                                myArray[2'b00] <= #1 32'h00000000; $display(";A 43");		//(assert (= myArray 0    0h00000000)) ;43
                            end
                        2'b01 :
                            begin
                                $display(";A 44");		//(assert (= addr    0b01)) ;44
                                myArray[2'b01] <= #1 32'h00000000; $display(";A 45");		//(assert (= myArray 1    0h00000000)) ;45
                            end
                        2'b10 :
                            begin
                                $display(";A 46");		//(assert (= addr    0b10)) ;46
                                myArray[2'b10] <= #1 32'h00000000; $display(";A 47");		//(assert (= myArray 2    0h00000000)) ;47
                            end
                        2'b11 :
                            begin
                                $display(";A 48");		//(assert (= addr    0b11)) ;48
                                myArray[2'b11] <= #1 32'h00000000; $display(";A 49");		//(assert (= myArray 3    0h00000000)) ;49
                            end
                    endcase
                end
            end
        end
        // Displaying module variables
        begin
            $display(";R addr = %b", addr);
            $display(";R cnt = %b", cnt);
            $display(";R myArray[0] = %b", myArray[0]);
            $display(";R myArray[1] = %b", myArray[1]);
            $display(";R myArray[2] = %b", myArray[2]);
            $display(";R myArray[3] = %b", myArray[3]);
            $display(";R out = %b", out);
            $display(";R out1 = %b", out1);
            $display(";R temp = %b", temp);
            $display(";R temp1 = %b", temp1);
        end
    end

endmodule

