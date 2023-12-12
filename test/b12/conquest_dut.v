// Following code segment is generated from /home/meng/Code/concolic-testing/test/b12/src/b12.v:1
module b12(clock, reset, start, k, nloss, nl, speaker, __obs);
    input clock;
    input reset;
    input start;
    input [3:0] k;
    output nloss;
    output [3:0] nl;
    output speaker;
    input __obs;

    reg [4:0] address = 5'b0;
    reg [1:0] count = 2'b0;
    reg [5:0] count2 = 6'b0;
    reg [2:0] counter = 3'b0;
    reg [1:0] data_in = 2'b0;
    reg [1:0] data_out = 2'b0;
    reg [4:0] gamma = 5'b0;
    reg [1:0] ind = 2'b0;
    reg [4:0] max = 5'b0;
    reg [1:0] memory [0:31];
    reg [3:0] nl = 4'b0;
    reg nloss = 1'b0;
    reg [1:0] num = 2'b0;
    reg play = 1'b0;
    reg s = 1'b0;
    reg [4:0] scan = 5'b0;
    reg [2:0] sound = 3'b0;
    reg speaker = 1'b0;
    reg [5:0] timebase = 6'b0;
    reg wr = 1'b0;

    // Following code segment is generated from /home/meng/Code/concolic-testing/test/b12/src/b12.v:96
    always @(posedge clock) begin
        if (reset) begin
            s = 1'sb0; $display(";A 2");		//(assert (= s    0b0)) ;2
            speaker <= #1 1'b0; $display(";A 3");		//(assert (= speaker    0b0)) ;3
            counter = 3'sb000; $display(";A 4");		//(assert (= counter    0b000)) ;4
        end
        else begin
            if (play) begin
                $display(";A 5");		//(assert (= play    0b1)) ;5
                case (sound)
                    3'b000 :
                        begin
                            $display(";A 7");		//(assert (= sound    0b000)) ;7
                            if ((counter > 32'h00000002)) begin
                                $display(";A 8");		//(assert (= (bool-to-bv (bv-gt counter  0h00000002))   0b1)) ;8
                                s = (~s); $display(";A 10");		//(assert (= s    (bv-not s ))) ;10
                                counter = 3'sb000; $display(";A 11");		//(assert (= counter    0b000)) ;11
                                speaker <= #1 s; $display(";A 12");		//(assert (= speaker    s )) ;12
                            end
                            else begin
                                $display(";A 9");		//(assert (= (bool-to-bv (bv-gt counter  0h00000002))   0b0)) ;9
                                counter = (counter + 3'b001); $display(";A 13");		//(assert (= counter    (bv-add counter  0b001))) ;13
                            end
                        end
                    3'b001 :
                        begin
                            $display(";A 14");		//(assert (= sound    0b001)) ;14
                            if ((counter > 32'h00000003)) begin
                                $display(";A 15");		//(assert (= (bool-to-bv (bv-gt counter  0h00000003))   0b1)) ;15
                                s = (~s); $display(";A 17");		//(assert (= s    (bv-not s ))) ;17
                                speaker <= #1 s; $display(";A 18");		//(assert (= speaker    s )) ;18
                                counter = 3'sb000; $display(";A 19");		//(assert (= counter    0b000)) ;19
                            end
                            else begin
                                $display(";A 16");		//(assert (= (bool-to-bv (bv-gt counter  0h00000003))   0b0)) ;16
                                counter = (counter + 3'b001); $display(";A 20");		//(assert (= counter    (bv-add counter  0b001))) ;20
                            end
                        end
                    3'b010 :
                        begin
                            $display(";A 21");		//(assert (= sound    0b010)) ;21
                            if ((counter > 32'h00000004)) begin
                                $display(";A 22");		//(assert (= (bool-to-bv (bv-gt counter  0h00000004))   0b1)) ;22
                                s = (~s); $display(";A 24");		//(assert (= s    (bv-not s ))) ;24
                                speaker <= #1 s; $display(";A 25");		//(assert (= speaker    s )) ;25
                                counter = 3'sb000; $display(";A 26");		//(assert (= counter    0b000)) ;26
                            end
                            else begin
                                $display(";A 23");		//(assert (= (bool-to-bv (bv-gt counter  0h00000004))   0b0)) ;23
                                counter = (counter + 3'b001); $display(";A 27");		//(assert (= counter    (bv-add counter  0b001))) ;27
                            end
                        end
                    3'b011 :
                        begin
                            $display(";A 28");		//(assert (= sound    0b011)) ;28
                            if ((counter > 32'h00000005)) begin
                                $display(";A 29");		//(assert (= (bool-to-bv (bv-gt counter  0h00000005))   0b1)) ;29
                                s = (~s); $display(";A 31");		//(assert (= s    (bv-not s ))) ;31
                                speaker <= #1 s; $display(";A 32");		//(assert (= speaker    s )) ;32
                                counter = 3'sb000; $display(";A 33");		//(assert (= counter    0b000)) ;33
                            end
                            else begin
                                $display(";A 30");		//(assert (= (bool-to-bv (bv-gt counter  0h00000005))   0b0)) ;30
                                counter = (counter + 3'b001); $display(";A 34");		//(assert (= counter    (bv-add counter  0b001))) ;34
                            end
                        end
                    3'b100 :
                        begin
                            $display(";A 35");		//(assert (= sound    0b100)) ;35
                            if ((counter > 32'h00000006)) begin
                                $display(";A 36");		//(assert (= (bool-to-bv (bv-gt counter  0h00000006))   0b1)) ;36
                                s = (~s); $display(";A 38");		//(assert (= s    (bv-not s ))) ;38
                                speaker <= #1 s; $display(";A 39");		//(assert (= speaker    s )) ;39
                                counter = 3'sb000; $display(";A 40");		//(assert (= counter    0b000)) ;40
                            end
                            else begin
                                $display(";A 37");		//(assert (= (bool-to-bv (bv-gt counter  0h00000006))   0b0)) ;37
                                counter = (counter + 3'b001); $display(";A 41");		//(assert (= counter    (bv-add counter  0b001))) ;41
                            end
                        end
                    3'b101 :
                        begin
                            $display(";A 42");		//(assert (= sound    0b101)) ;42
                            if ((counter > 32'h00000001)) begin
                                $display(";A 43");		//(assert (= (bool-to-bv (bv-gt counter  0h00000001))   0b1)) ;43
                                s = (~s); $display(";A 45");		//(assert (= s    (bv-not s ))) ;45
                                speaker <= #1 s; $display(";A 46");		//(assert (= speaker    s )) ;46
                                counter = 3'sb000; $display(";A 47");		//(assert (= counter    0b000)) ;47
                            end
                            else begin
                                $display(";A 44");		//(assert (= (bool-to-bv (bv-gt counter  0h00000001))   0b0)) ;44
                                counter = (counter + 3'b001); $display(";A 48");		//(assert (= counter    (bv-add counter  0b001))) ;48
                            end
                        end
                    default:
                        begin
                            $display(";A 49");		//(assert (= (and (/= sound  0b000) (/= sound  0b001) (/= sound  0b010) (/= sound  0b011) (/= sound  0b100) (/= sound  0b101))   true)) ;49
                            counter = 3'sb000; $display(";A 50");		//(assert (= counter    0b000)) ;50
                        end
                endcase
            end
            else begin
                $display(";A 6");		//(assert (= play    0b0)) ;6
                counter = 3'sb000; $display(";A 51");		//(assert (= counter    0b000)) ;51
                speaker <= #1 1'b0; $display(";A 52");		//(assert (= speaker    0b0)) ;52
            end
        end
    end

    // Following code segment is generated from /home/meng/Code/concolic-testing/test/b12/src/b12.v:195
    always @(posedge clock) begin
        if (reset) begin
            count = 2'sb00; $display(";A 55");		//(assert (= count    0b00)) ;55
            num <= #1 2'b00; $display(";A 56");		//(assert (= num    0b00)) ;56
        end
        else begin
            if ((count == 2'b11)) begin
                $display(";A 57");		//(assert (= (bv-comp count  0b11)   0b1)) ;57
                count = 2'sb00; $display(";A 59");		//(assert (= count    0b00)) ;59
            end
            else begin
                $display(";A 58");		//(assert (= (bv-comp count  0b11)   0b0)) ;58
                count = (count + 2'b01); $display(";A 60");		//(assert (= count    (bv-add count  0b01))) ;60
            end
            num <= #1 count; $display(";A 61");		//(assert (= num    count )) ;61
        end
    end

    // Following code segment is generated from /home/meng/Code/concolic-testing/test/b12/src/b12.v:217
    always @(posedge clock) begin
        if (reset) begin
            data_out <= #1 2'b00; $display(";A 64");		//(assert (= data_out    0b00)) ;64
            memory[5'b00000] = 2'sb00; $display(";A 65");		//(assert (= memory 0    0b00)) ;65
            memory[5'b00001] = 2'sb00; $display(";A 66");		//(assert (= memory 1    0b00)) ;66
            memory[5'b00010] = 2'sb00; $display(";A 67");		//(assert (= memory 2    0b00)) ;67
            memory[5'b00011] = 2'sb00; $display(";A 68");		//(assert (= memory 3    0b00)) ;68
            memory[5'b00100] = 2'sb00; $display(";A 69");		//(assert (= memory 4    0b00)) ;69
            memory[5'b00101] = 2'sb00; $display(";A 70");		//(assert (= memory 5    0b00)) ;70
            memory[5'b00110] = 2'sb00; $display(";A 71");		//(assert (= memory 6    0b00)) ;71
            memory[5'b00111] = 2'sb00; $display(";A 72");		//(assert (= memory 7    0b00)) ;72
            memory[5'b01000] = 2'sb00; $display(";A 73");		//(assert (= memory 8    0b00)) ;73
            memory[5'b01001] = 2'sb00; $display(";A 74");		//(assert (= memory 9    0b00)) ;74
            memory[5'b01010] = 2'sb00; $display(";A 75");		//(assert (= memory 10    0b00)) ;75
            memory[5'b01011] = 2'sb00; $display(";A 76");		//(assert (= memory 11    0b00)) ;76
            memory[5'b01100] = 2'sb00; $display(";A 77");		//(assert (= memory 12    0b00)) ;77
            memory[5'b01101] = 2'sb00; $display(";A 78");		//(assert (= memory 13    0b00)) ;78
            memory[5'b01110] = 2'sb00; $display(";A 79");		//(assert (= memory 14    0b00)) ;79
            memory[5'b01111] = 2'sb00; $display(";A 80");		//(assert (= memory 15    0b00)) ;80
            memory[5'b10000] = 2'sb00; $display(";A 81");		//(assert (= memory 16    0b00)) ;81
            memory[5'b10001] = 2'sb00; $display(";A 82");		//(assert (= memory 17    0b00)) ;82
            memory[5'b10010] = 2'sb00; $display(";A 83");		//(assert (= memory 18    0b00)) ;83
            memory[5'b10011] = 2'sb00; $display(";A 84");		//(assert (= memory 19    0b00)) ;84
            memory[5'b10100] = 2'sb00; $display(";A 85");		//(assert (= memory 20    0b00)) ;85
            memory[5'b10101] = 2'sb00; $display(";A 86");		//(assert (= memory 21    0b00)) ;86
            memory[5'b10110] = 2'sb00; $display(";A 87");		//(assert (= memory 22    0b00)) ;87
            memory[5'b10111] = 2'sb00; $display(";A 88");		//(assert (= memory 23    0b00)) ;88
            memory[5'b11000] = 2'sb00; $display(";A 89");		//(assert (= memory 24    0b00)) ;89
            memory[5'b11001] = 2'sb00; $display(";A 90");		//(assert (= memory 25    0b00)) ;90
            memory[5'b11010] = 2'sb00; $display(";A 91");		//(assert (= memory 26    0b00)) ;91
            memory[5'b11011] = 2'sb00; $display(";A 92");		//(assert (= memory 27    0b00)) ;92
            memory[5'b11100] = 2'sb00; $display(";A 93");		//(assert (= memory 28    0b00)) ;93
            memory[5'b11101] = 2'sb00; $display(";A 94");		//(assert (= memory 29    0b00)) ;94
            memory[5'b11110] = 2'sb00; $display(";A 95");		//(assert (= memory 30    0b00)) ;95
            memory[5'b11111] = 2'sb00; $display(";A 96");		//(assert (= memory 31    0b00)) ;96
        end
        else begin
            data_out <= #1 memory[address]; $display(";A 97");		//(assert (= data_out    ( memory address ))) ;97
            if (wr) begin
                $display(";A 98");		//(assert (= wr    0b1)) ;98
                memory[address] = 