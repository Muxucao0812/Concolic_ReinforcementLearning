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

    // Following code segment is generated from /home/meng/Code/concolic-testing/test/b12/src/b12.v:68
    always @(posedge clock) begin
        if (reset) begin
            s = 1'b0; $display(";A 2");		//(assert (= s    0b0)) ;2
            speaker <= #1 1'b0; $display(";A 3");		//(assert (= speaker    0b0)) ;3
            counter = 3'b000; $display(";A 4");		//(assert (= counter    0b000)) ;4
        end
        else begin
            if (play) begin
                $display(";A 5");		//(assert (= play    0b1)) ;5
                case (sound)
                    3'b000 :
                        begin
                            $display(";A 7");		//(assert (= sound    0b000)) ;7
                            if ((counter > 3'b010)) begin
                                $display(";A 8");		//(assert (= (bool-to-bv (bv-gt counter  0b010))   0b1)) ;8
                                s = (~s); $display(";A 10");		//(assert (= s    (bv-not s ))) ;10
                                counter = 3'b000; $display(";A 11");		//(assert (= counter    0b000)) ;11
                                speaker <= #1 s; $display(";A 12");		//(assert (= speaker    s )) ;12
                            end
                            else begin
                                $display(";A 9");		//(assert (= (bool-to-bv (bv-gt counter  0b010))   0b0)) ;9
                                counter = (counter + 3'b001); $display(";A 13");		//(assert (= counter    (bv-add counter  0b001))) ;13
                            end
                        end
                    3'b001 :
                        begin
                            $display(";A 14");		//(assert (= sound    0b001)) ;14
                            if ((counter > 3'b011)) begin
                                $display(";A 15");		//(assert (= (bool-to-bv (bv-gt counter  0b011))   0b1)) ;15
                                s = (~s); $display(";A 17");		//(assert (= s    (bv-not s ))) ;17
                                speaker <= #1 s; $display(";A 18");		//(assert (= speaker    s )) ;18
                                counter = 3'b000; $display(";A 19");		//(assert (= counter    0b000)) ;19
                            end
                            else begin
                                $display(";A 16");		//(assert (= (bool-to-bv (bv-gt counter  0b011))   0b0)) ;16
                                counter = (counter + 3'b001); $display(";A 20");		//(assert (= counter    (bv-add counter  0b001))) ;20
                            end
                        end
                    3'b010 :
                        begin
                            $display(";A 21");		//(assert (= sound    0b010)) ;21
                            if ((counter > 3'b100)) begin
                                $display(";A 22");		//(assert (= (bool-to-bv (bv-gt counter  0b100))   0b1)) ;22
                                s = (~s); $display(";A 24");		//(assert (= s    (bv-not s ))) ;24
                                speaker <= #1 s; $display(";A 25");		//(assert (= speaker    s )) ;25
                                counter = 3'b000; $display(";A 26");		//(assert (= counter    0b000)) ;26
                            end
                            else begin
                                $display(";A 23");		//(assert (= (bool-to-bv (bv-gt counter  0b100))   0b0)) ;23
                                counter = (counter + 3'b001); $display(";A 27");		//(assert (= counter    (bv-add counter  0b001))) ;27
                            end
                        end
                    3'b011 :
                        begin
                            $display(";A 28");		//(assert (= sound    0b011)) ;28
                            if ((counter > 3'b101)) begin
                                $display(";A 29");		//(assert (= (bool-to-bv (bv-gt counter  0b101))   0b1)) ;29
                                s = (~s); $display(";A 31");		//(assert (= s    (bv-not s ))) ;31
                                speaker <= #1 s; $display(";A 32");		//(assert (= speaker    s )) ;32
                                counter = 3'b000; $display(";A 33");		//(assert (= counter    0b000)) ;33
                            end
                            else begin
                                $display(";A 30");		//(assert (= (bool-to-bv (bv-gt counter  0b101))   0b0)) ;30
                                counter = (counter + 3'b001); $display(";A 34");		//(assert (= counter    (bv-add counter  0b001))) ;34
                            end
                        end
                    3'b100 :
                        begin
                            $display(";A 35");		//(assert (= sound    0b100)) ;35
                            if ((counter > 3'b110)) begin
                                $display(";A 36");		//(assert (= (bool-to-bv (bv-gt counter  0b110))   0b1)) ;36
                                s = (~s); $display(";A 38");		//(assert (= s    (bv-not s ))) ;38
                                speaker <= #1 s; $display(";A 39");		//(assert (= speaker    s )) ;39
                                counter = 3'b000; $display(";A 40");		//(assert (= counter    0b000)) ;40
                            end
                            else begin
                                $display(";A 37");		//(assert (= (bool-to-bv (bv-gt counter  0b110))   0b0)) ;37
                                counter = (counter + 3'b001); $display(";A 41");		//(assert (= counter    (bv-add counter  0b001))) ;41
                            end
                        end
                    3'b101 :
                        begin
                            $display(";A 42");		//(assert (= sound    0b101)) ;42
                            if ((counter > 3'b001)) begin
                                $display(";A 43");		//(assert (= (bool-to-bv (bv-gt counter  0b001))   0b1)) ;43
                                s = (~s); $display(";A 45");		//(assert (= s    (bv-not s ))) ;45
                                speaker <= #1 s; $display(";A 46");		//(assert (= speaker    s )) ;46
                                counter = 3'b000; $display(";A 47");		//(assert (= counter    0b000)) ;47
                            end
                            else begin
                                $display(";A 44");		//(assert (= (bool-to-bv (bv-gt counter  0b001))   0b0)) ;44
                                counter = (counter + 3'b001); $display(";A 48");		//(assert (= counter    (bv-add counter  0b001))) ;48
                            end
                        end
                    default:
                        begin
                            $display(";A 49");		//(assert (= (and (/= sound  0b000) (/= sound  0b001) (/= sound  0b010) (/= sound  0b011) (/= sound  0b100) (/= sound  0b101))   true)) ;49
                            counter = 3'b000; $display(";A 50");		//(assert (= counter    0b000)) ;50
                        end
                endcase
            end
            else begin
                $display(";A 6");		//(assert (= play    0b0)) ;6
                counter = 3'b000; $display(";A 51");		//(assert (= counter    0b000)) ;51
                speaker <= #1 1'b0; $display(";A 52");		//(assert (= speaker    0b0)) ;52
            end
        end
    end

    // Following code segment is generated from /home/meng/Code/concolic-testing/test/b12/src/b12.v:167
    always @(posedge clock) begin
        if (reset) begin
            count = 2'b00; $display(";A 55");		//(assert (= count    0b00)) ;55
            num <= #1 2'b00; $display(";A 56");		//(assert (= num    0b00)) ;56
        end
        else begin
            if ((count == 2'b11)) begin
                $display(";A 57");		//(assert (= (bv-comp count  0b11)   0b1)) ;57
                count = 2'b00; $display(";A 59");		//(assert (= count    0b00)) ;59
            end
            else begin
                $display(";A 58");		//(assert (= (bv-comp count  0b11)   0b0)) ;58
                count = (count + 2'b01); $display(";A 60");		//(assert (= count    (bv-add count  0b01))) ;60
            end
            num <= #1 count; $display(";A 61");		//(assert (= num    count )) ;61
        end
    end

    // Following code segment is generated from /home/meng/Code/concolic-testing/test/b12/src/b12.v:189
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
                memory[address] = data_in; $display(";A 100");		//(assert (= ( memory address )   data_in )) ;100
            end
            else begin
                $display(";A 99");		//(assert (= wr    0b0)) ;99
            end
        end
    end

    // Following code segment is generated from /home/meng/Code/concolic-testing/test/b12/src/b12.v:251
    always @(posedge clock) begin
        if (reset) begin
            nloss <= #1 1'b0; $display(";A 103");		//(assert (= nloss    0b0)) ;103
            nl <= #1 4'h0; $display(";A 104");		//(assert (= nl    0h0)) ;104
            play <= #1 1'b0; $display(";A 105");		//(assert (= play    0b0)) ;105
            wr <= #1 1'b0; $display(";A 106");		//(assert (= wr    0b0)) ;106
            scan = 5'b00000; $display(";A 107");		//(assert (= scan    0b00000)) ;107
            max = 5'b00000; $display(";A 108");		//(assert (= max    0b00000)) ;108
            ind = 2'b00; $display(";A 109");		//(assert (= ind    0b00)) ;109
            timebase = 6'b000000; $display(";A 110");		//(assert (= timebase    0b000000)) ;110
            count2 = 6'b000000; $display(";A 111");		//(assert (= count2    0b000000)) ;111
            sound <= #1 3'b000; $display(";A 112");		//(assert (= sound    0b000)) ;112
            address <= #1 5'b00000; $display(";A 113");		//(assert (= address    0b00000)) ;113
            data_in <= #1 2'b00; $display(";A 114");		//(assert (= data_in    0b00)) ;114
            gamma = 5'b00000; $display(";A 115");		//(assert (= gamma    0b00000)) ;115
        end
        else begin
            if ((start == 1'b1)) begin
                $display(";A 116");		//(assert (= (bv-comp start  0b1)   0b1)) ;116
                gamma = 5'b00001; $display(";A 118");		//(assert (= gamma    0b00001)) ;118
            end
            else begin
                $display(";A 117");		//(assert (= (bv-comp start  0b1)   0b0)) ;117
                gamma = gamma; $display(";A 119");		//(assert (= gamma    gamma )) ;119
            end
            case (gamma)
                5'b00000 :
                    begin
                        $display(";A 120");		//(assert (= gamma    0b00000)) ;120
                        gamma = 5'b00000; $display(";A 121");		//(assert (= gamma    0b00000)) ;121
                    end
                5'b00001 :
                    begin
                        $display(";A 122");		//(assert (= gamma    0b00001)) ;122
                        nloss <= #1 1'b0; $display(";A 123");		//(assert (= nloss    0b0)) ;123
                        nl <= #1 4'h0; $display(";A 124");		//(assert (= nl    0h0)) ;124
                        play <= #1 1'b0; $display(";A 125");		//(assert (= play    0b0)) ;125
                        wr <= #1 1'b0; $display(";A 126");		//(assert (= wr    0b0)) ;126
                        max = 5'b00000; $display(";A 127");		//(assert (= max    0b00000)) ;127
                        timebase = 6'b100001; $display(";A 128");		//(assert (= timebase    0b100001)) ;128
                        gamma = 5'b00010; $display(";A 129");		//(assert (= gamma    0b00010)) ;129
                    end
                5'b00010 :
                    begin
                        $display(";A 130");		//(assert (= gamma    0b00010)) ;130
                        scan = 5'b00000; $display(";A 131");		//(assert (= scan    0b00000)) ;131
                        wr <= #1 1'b1; $display(";A 132");		//(assert (= wr    0b1)) ;132
                        address <= #1 max; $display(";A 133");		//(assert (= address    max )) ;133
                        data_in <= #1 num; $display(";A 134");		//(assert (= data_in    num )) ;134
                        gamma = 5'b00011; $display(";A 135");		//(assert (= gamma    0b00011)) ;135
                    end
                5'b00011 :
                    begin
                        $display(";A 136");		//(assert (= gamma    0b00011)) ;136
                        wr <= #1 1'b0; $display(";A 137");		//(assert (= wr    0b0)) ;137
                        address <= #1 scan; $display(";A 138");		//(assert (= address    scan )) ;138
                        gamma = 5'b00100; $display(";A 139");		//(assert (= gamma    0b00100)) ;139
                    end
                5'b00100 :
                    begin
                        $display(";A 140");		//(assert (= gamma    0b00100)) ;140
                        gamma = 5'b00101; $display(";A 141");		//(assert (= gamma    0b00101)) ;141
                    end
                5'b00101 :
                    begin
                        $display(";A 142");		//(assert (= gamma    0b00101)) ;142
                        nl[data_out] <= #1 1'sb1; $display(";A 143");		//(assert (= (bv-extract 0 0 nl )   0b1)) ;143
                        count2 = timebase; $display(";A 144");		//(assert (= count2    timebase )) ;144
                        play <= #1 1'b1; $display(";A 145");		//(assert (= play    0b1)) ;145
                        sound <= #1 {1'b0, data_out}; $display(";A 146");		//(assert (= sound    (bv-concat 0b0 data_out ))) ;146
                        gamma = 5'b00110; $display(";A 147");		//(assert (= gamma    0b00110)) ;147
                    end
                5'b00110 :
                    begin
                        $display(";A 148");		//(assert (= gamma    0b00110)) ;148
                        if ((count2 == 6'b000000)) begin
                            $display(";A 149");		//(assert (= (bv-comp count2  0b000000)   0b1)) ;149
                            nl <= #1 4'h0; $display(";A 151");		//(assert (= nl    0h0)) ;151
                            play <= #1 1'b0; $display(";A 152");		//(assert (= play    0b0)) ;152
                            count2 = timebase; $display(";A 153");		//(assert (= count2    timebase )) ;153
                            gamma = 5'b00111; $display(";A 154");		//(assert (= gamma    0b00111)) ;154
                        end
                        else begin
                            $display(";A 150");		//(assert (= (bv-comp count2  0b000000)   0b0)) ;150
                            count2 = (count2 - 6'b000001); $display(";A 155");		//(assert (= count2    (bv-sub count2  0b000001))) ;155
                            gamma = 5'b00110; $display(";A 156");		//(assert (= gamma    0b00110)) ;156
                        end
                    end
                5'b00111 :
                    begin
                        $display(";A 157");		//(assert (= gamma    0b00111)) ;157
                        if ((count2 == 6'b000000)) begin
                            $display(";A 158");		//(assert (= (bv-comp count2  0b000000)   0b1)) ;158
                            if ((scan != max)) begin
                                $display(";A 160");		//(assert (= (bv-not (bv-comp scan  max ))   0b1)) ;160
                                scan = (scan + 5'b00001); $display(";A 162");		//(assert (= scan    (bv-add scan  0b00001))) ;162
                                gamma = 5'b00011; $display(";A 163");		//(assert (= gamma    0b00011)) ;163
                            end
                            else begin
                                $display(";A 161");		//(assert (= (bv-not (bv-comp scan  max ))   0b0)) ;161
                                scan = 5'b00000; $display(";A 164");		//(assert (= scan    0b00000)) ;164
                                gamma = 5'b01000; $display(";A 165");		//(assert (= gamma    0b01000)) ;165
                            end
                        end
                        else begin
                            $display(";A 159");		//(assert (= (bv-comp count2  0b000000)   0b0)) ;159
                            count2 = (count2 - 6'b000001); $display(";A 166");		//(assert (= count2    (bv-sub count2  0b000001))) ;166
                            gamma = 5'b00111; $display(";A 167");		//(assert (= gamma    0b00111)) ;167
                        end
                    end
                5'b01000 :
                    begin
                        $display(";A 168");		//(assert (= gamma    0b01000)) ;168
                        count2 = 6'b100001; $display(";A 169");		//(assert (= count2    0b100001)) ;169
                        address <= #1 scan; $display(";A 170");		//(assert (= address    scan )) ;170
                        gamma = 5'b01001; $display(";A 171");		//(assert (= gamma    0b01001)) ;171
                    end
                5'b01001 :
                    begin
                        $display(";A 172");		//(assert (= gamma    0b01001)) ;172
                        gamma = 5'b01010; $display(";A 173");		//(assert (= gamma    0b01010)) ;173
                    end
                5'b01010 :
                    begin
                        $display(";A 174");		//(assert (= gamma    0b01010)) ;174
                        if ((count2 == 6'b000000)) begin
                            $display(";A 175");		//(assert (= (bv-comp count2  0b000000)   0b1)) ;175
                            nloss <= #1 1'b1; $display(";A 177");		//(assert (= nloss    0b1)) ;177
                            max = 5'b00000; $display(";A 178");		//(assert (= max    0b00000)) ;178
                            gamma = 5'b10001; $display(";A 179");		//(assert (= gamma    0b10001)) ;179
                        end
                        else begin
                            $display(";A 176");		//(assert (= (bv-comp count2  0b000000)   0b0)) ;176
                            count2 = (count2 - 6'b000001); $display(";A 180");		//(assert (= count2    (bv-sub count2  0b000001))) ;180
                            if ((k[0] == 1'b1)) begin
                                $display(";A 181");		//(assert (= (bv-comp (bv-extract 0 0 k ) 0b1)   0b1)) ;181
                                ind = 2'b00; $display(";A 183");		//(assert (= ind    0b00)) ;183
                                sound <= #1 3'b000; $display(";A 184");		//(assert (= sound    0b000)) ;184
                                play <= #1 1'b1; $display(";A 185");		//(assert (= play    0b1)) ;185
                                count2 = timebase; $display(";A 186");		//(assert (= count2    timebase )) ;186
                                if ((data_out == 2'b00)) begin
                                    $display(";A 187");		//(assert (= (bv-comp data_out  0b00)   0b1)) ;187
                                    gamma = 5'b01011; $display(";A 189");		//(assert (= gamma    0b01011)) ;189
                                end
                                else begin
                                    $display(";A 188");		//(assert (= (bv-comp data_out  0b00)   0b0)) ;188
                                    nloss <= #1 1'b1; $display(";A 190");		//(assert (= nloss    0b1)) ;190
                                    gamma = 5'b01110; $display(";A 191");		//(assert (= gamma    0b01110)) ;191
                                end
                            end
                            else begin
                                $display(";A 182");		//(assert (= (bv-comp (bv-extract 0 0 k ) 0b1)   0b0)) ;182
                                if ((k[1] == 1'b1)) begin
                                    $display(";A 192");		//(assert (= (bv-comp (bv-extract 1 1 k ) 0b1)   0b1)) ;192
                                    ind = 2'b01; $display(";A 194");		//(assert (= ind    0b01)) ;194
                                    sound <= #1 3'b001; $display(";A 195");		//(assert (= sound    0b001)) ;195
                                    play <= #1 1'b1; $display(";A 196");		//(assert (= play    0b1)) ;196
                                    count2 = timebase; $display(";A 197");		//(assert (= count2    timebase )) ;197
                                    if ((data_out == 2'b01)) begin
                                        $display(";A 198");		//(assert (= (bv-comp data_out  0b01)   0b1)) ;198
                                        gamma = 5'b01011; $display(";A 200");		//(assert (= gamma    0b01011)) ;200
                                    end
                                    else begin
                                        $display(";A 199");		//(assert (= (bv-comp data_out  0b01)   0b0)) ;199
                                        nloss <= #1 1'b1; $display(";A 201");		//(assert (= nloss    0b1)) ;201
                                        gamma = 5'b01110; $display(";A 202");		//(assert (= gamma    0b01110)) ;202
                                    end
                                end
                                else begin
                                    $display(";A 193");		//(assert (= (bv-comp (bv-extract 1 1 k ) 0b1)   0b0)) ;193
                                    if ((k[2] == 1'b1)) begin
                                        $display(";A 203");		//(assert (= (bv-comp (bv-extract 2 2 k ) 0b1)   0b1)) ;203
                                        ind = 2'b10; $display(";A 205");		//(assert (= ind    0b10)) ;205
                                        sound <= #1 3'b010; $display(";A 206");		//(assert (= sound    0b010)) ;206
                                        play <= #1 1'b1; $display(";A 207");		//(assert (= play    0b1)) ;207
                                        count2 = timebase; $display(";A 208");		//(assert (= count2    timebase )) ;208
                                        if ((data_out == 2'b10)) begin
                                            $display(";A 209");		//(assert (= (bv-comp data_out  0b10)   0b1)) ;209
                                            gamma = 5'b01011; $display(";A 211");		//(assert (= gamma    0b01011)) ;211
                                        end
                                        else begin
                                            $display(";A 210");		//(assert (= (bv-comp data_out  0b10)   0b0)) ;210
                                            nloss <= #1 1'b1; $display(";A 212");		//(assert (= nloss    0b1)) ;212
                                            gamma = 5'b01110; $display(";A 213");		//(assert (= gamma    0b01110)) ;213
                                        end
                                    end
                                    else begin
                                        $display(";A 204");		//(assert (= (bv-comp (bv-extract 2 2 k ) 0b1)   0b0)) ;204
                                        if ((k[3] == 1'b1)) begin
                                            $display(";A 214");		//(assert (= (bv-comp (bv-extract 3 3 k ) 0b1)   0b1)) ;214
                                            ind = 2'b11; $display(";A 216");		//(assert (= ind    0b11)) ;216
                                            sound <= #1 3'b011; $display(";A 217");		//(assert (= sound    0b011)) ;217
                                            play <= #1 1'b1; $display(";A 218");		//(assert (= play    0b1)) ;218
                                            count2 = timebase; $display(";A 219");		//(assert (= count2    timebase )) ;219
                                            if ((data_out == 2'b11)) begin
                                                $display(";A 220");		//(assert (= (bv-comp data_out  0b11)   0b1)) ;220
                                                gamma = 5'b01011; $display(";A 222");		//(assert (= gamma    0b01011)) ;222
                                            end
                                            else begin
                                                $display(";A 221");		//(assert (= (bv-comp data_out  0b11)   0b0)) ;221
                                                nloss <= #1 1'b1; $display(";A 223");		//(assert (= nloss    0b1)) ;223
                                                gamma = 5'b01110; $display(";A 224");		//(assert (= gamma    0b01110)) ;224
                                            end
                                        end
                                        else begin
                                            $display(";A 215");		//(assert (= (bv-comp (bv-extract 3 3 k ) 0b1)   0b0)) ;215
                                            gamma = 5'b01010; $display(";A 225");		//(assert (= gamma    0b01010)) ;225
                                        end
                                    end
                                end
                            end
                        end
                    end
                5'b01011 :
                    begin
                        $display(";A 226");		//(assert (= gamma    0b01011)) ;226
                        nl[ind] <= #1 1'sb1; $display(";A 227");		//(assert (= (bv-extract 0 0 nl )   0b1)) ;227
                        gamma = 5'b01100; $display(";A 228");		//(assert (= gamma    0b01100)) ;228
                    end
                5'b01100 :
                    begin
                        $display(";A 229");		//(assert (= gamma    0b01100)) ;229
                        if ((count2 == 6'b000000)) begin
                            $display(";A 230");		//(assert (= (bv-comp count2  0b000000)   0b1)) ;230
                            nl <= #1 4'h0; $display(";A 232");		//(assert (= nl    0h0)) ;232
                            play <= #1 1'b0; $display(";A 233");		//(assert (= play    0b0)) ;233
                            count2 = timebase; $display(";A 234");		//(assert (= count2    timebase )) ;234
                            gamma = 5'b01101; $display(";A 235");		//(assert (= gamma    0b01101)) ;235
                        end
                        else begin
                            $display(";A 231");		//(assert (= (bv-comp count2  0b000000)   0b0)) ;231
                            count2 = (count2 - 6'b000001); $display(";A 236");		//(assert (= count2    (bv-sub count2  0b000001))) ;236
                            gamma = 5'b01100; $display(";A 237");		//(assert (= gamma    0b01100)) ;237
                        end
                    end
                5'b01101 :
                    begin
                        $display(";A 238");		//(assert (= gamma    0b01101)) ;238
                        if ((count2 == 6'b000000)) begin
                            $display(";A 239");		//(assert (= (bv-comp count2  0b000000)   0b1)) ;239
                            if ((scan != max)) begin
                                $display(";A 241");		//(assert (= (bv-not (bv-comp scan  max ))   0b1)) ;241
                                scan = (scan + 5'b00001); $display(";A 243");		//(assert (= scan    (bv-add scan  0b00001))) ;243
                                gamma = 5'b01000; $display(";A 244");		//(assert (= gamma    0b01000)) ;244
                            end
                            else begin
                                $display(";A 242");		//(assert (= (bv-not (bv-comp scan  max ))   0b0)) ;242
                                if ((max != 5'b11111)) begin
                                    $display(";A 245");		//(assert (= (bv-not (bv-comp max  0b11111))   0b1)) ;245
                                    max = (max + 5'b00001); $display(";A 247");		//(assert (= max    (bv-add max  0b00001))) ;247
                                    timebase = (timebase - 6'b000001); $display(";A 248");		//(assert (= timebase    (bv-sub timebase  0b000001))) ;248
                                    gamma = 5'b00010; $display(";A 249");		//(assert (= gamma    0b00010)) ;249
                                end
                                else begin
                                    $display(";A 246");		//(assert (= (bv-not (bv-comp max  0b11111))   0b0)) ;246
                                    play <= #1 1'b1; $display(";A 250");		//(assert (= play    0b1)) ;250
                                    sound <= #1 3'b100; $display(";A 251");		//(assert (= sound    0b100)) ;251
                                    count2 = 6'b001000; $display(";A 252");		//(assert (= count2    0b001000)) ;252
                                    gamma = 5'b11000; $display(";A 253");		//(assert (= gamma    0b11000)) ;253
                                end
                            end
                        end
                        else begin
                            $display(";A 240");		//(assert (= (bv-comp count2  0b000000)   0b0)) ;240
                            count2 = (count2 - 6'b000001); $display(";A 254");		//(assert (= count2    (bv-sub count2  0b000001))) ;254
                            gamma = 5'b01101; $display(";A 255");		//(assert (= gamma    0b01101)) ;255
                        end
                    end
                5'b01110 :
                    begin
                        $display(";A 256");		//(assert (= gamma    0b01110)) ;256
                        nl[ind] <= #1 1'sb1; $display(";A 257");		//(assert (= (bv-extract 0 0 nl )   0b1)) ;257
                        gamma = 5'b01111; $display(";A 258");		//(assert (= gamma    0b01111)) ;258
                    end
                5'b01111 :
                    begin
                        $display(";A 259");		//(assert (= gamma    0b01111)) ;259
                        if ((count2 == 6'b000000)) begin
                            $display(";A 260");		//(assert (= (bv-comp count2  0b000000)   0b1)) ;260
                            nl <= #1 4'h0; $display(";A 262");		//(assert (= nl    0h0)) ;262
                            play <= #1 1'b0; $display(";A 263");		//(assert (= play    0b0)) ;263
                            count2 = timebase; $display(";A 264");		//(assert (= count2    timebase )) ;264
                            gamma = 5'b10000; $display(";A 265");		//(assert (= gamma    0b10000)) ;265
                        end
                        else begin
                            $display(";A 261");		//(assert (= (bv-comp count2  0b000000)   0b0)) ;261
                            count2 = (count2 - 6'b000001); $display(";A 266");		//(assert (= count2    (bv-sub count2  0b000001))) ;266
                            gamma = 5'b01111; $display(";A 267");		//(assert (= gamma    0b01111)) ;267
                        end
                    end
                5'b10000 :
                    begin
                        $display(";A 268");		//(assert (= gamma    0b10000)) ;268
                        if ((count2 == 6'b000000)) begin
                            $display(";A 269");		//(assert (= (bv-comp count2  0b000000)   0b1)) ;269
                            max = 5'b00000; $display(";A 271");		//(assert (= max    0b00000)) ;271
                            gamma = 5'b10001; $display(";A 272");		//(assert (= gamma    0b10001)) ;272
                        end
                        else begin
                            $display(";A 270");		//(assert (= (bv-comp count2  0b000000)   0b0)) ;270
                            count2 = (count2 - 6'b000001); $display(";A 273");		//(assert (= count2    (bv-sub count2  0b000001))) ;273
                            gamma = 5'b10000; $display(";A 274");		//(assert (= gamma    0b10000)) ;274
                        end
                    end
                5'b10001 :
                    begin
                        $display(";A 275");		//(assert (= gamma    0b10001)) ;275
                        address <= #1 max; $display(";A 276");		//(assert (= address    max )) ;276
                        gamma = 5'b10010; $display(";A 277");		//(assert (= gamma    0b10010)) ;277
                    end
                5'b10010 :
                    begin
                        $display(";A 278");		//(assert (= gamma    0b10010)) ;278
                        gamma = 5'b10011; $display(";A 279");		//(assert (= gamma    0b10011)) ;279
                    end
                5'b10011 :
                    begin
                        $display(";A 280");		//(assert (= gamma    0b10011)) ;280
                        nl[data_out] <= #1 1'sb1; $display(";A 281");		//(assert (= (bv-extract 0 0 nl )   0b1)) ;281
                        play <= #1 1'b1; $display(";A 282");		//(assert (= play    0b1)) ;282
                        sound <= #1 {1'b0, data_out}; $display(";A 283");		//(assert (= sound    (bv-concat 0b0 data_out ))) ;283
                        count2 = timebase; $display(";A 284");		//(assert (= count2    timebase )) ;284
                        gamma = 5'b10100; $display(";A 285");		//(assert (= gamma    0b10100)) ;285
                    end
                5'b10100 :
                    begin
                        $display(";A 286");		//(assert (= gamma    0b10100)) ;286
                        if ((count2 == 6'b000000)) begin
                            $display(";A 287");		//(assert (= (bv-comp count2  0b000000)   0b1)) ;287
                            nl <= #1 4'h0; $display(";A 289");		//(assert (= nl    0h0)) ;289
                            play <= #1 1'b0; $display(";A 290");		//(assert (= play    0b0)) ;290
                            count2 = timebase; $display(";A 291");		//(assert (= count2    timebase )) ;291
                            gamma = 5'b10101; $display(";A 292");		//(assert (= gamma    0b10101)) ;292
                        end
                        else begin
                            $display(";A 288");		//(assert (= (bv-comp count2  0b000000)   0b0)) ;288
                            count2 = (count2 - 6'b000001); $display(";A 293");		//(assert (= count2    (bv-sub count2  0b000001))) ;293
                            gamma = 5'b10100; $display(";A 294");		//(assert (= gamma    0b10100)) ;294
                        end
                    end
                5'b10101 :
                    begin
                        $display(";A 295");		//(assert (= gamma    0b10101)) ;295
                        if ((count2 == 6'b000000)) begin
                            $display(";A 296");		//(assert (= (bv-comp count2  0b000000)   0b1)) ;296
                            if ((max != scan)) begin
                                $display(";A 298");		//(assert (= (bv-not (bv-comp max  scan ))   0b1)) ;298
                                max = (max + 5'b00001); $display(";A 300");		//(assert (= max    (bv-add max  0b00001))) ;300
                                gamma = 5'b10001; $display(";A 301");		//(assert (= gamma    0b10001)) ;301
                            end
                            else begin
                                $display(";A 299");		//(assert (= (bv-not (bv-comp max  scan ))   0b0)) ;299
                                nl[data_out] <= #1 1'sb1; $display(";A 302");		//(assert (= (bv-extract 0 0 nl )   0b1)) ;302
                                play <= #1 1'b1; $display(";A 303");		//(assert (= play    0b1)) ;303
                                sound <= #1 3'b101; $display(";A 304");		//(assert (= sound    0b101)) ;304
                                count2 = 6'b001000; $display(";A 305");		//(assert (= count2    0b001000)) ;305
                                gamma = 5'b10110; $display(";A 306");		//(assert (= gamma    0b10110)) ;306
                            end
                        end
                        else begin
                            $display(";A 297");		//(assert (= (bv-comp count2  0b000000)   0b0)) ;297
                            count2 = (count2 - 6'b000001); $display(";A 307");		//(assert (= count2    (bv-sub count2  0b000001))) ;307
                            gamma = 5'b10101; $display(";A 308");		//(assert (= gamma    0b10101)) ;308
                        end
                    end
                5'b10110 :
                    begin
                        $display(";A 309");		//(assert (= gamma    0b10110)) ;309
                        if ((count2 == 6'b000000)) begin
                            $display(";A 310");		//(assert (= (bv-comp count2  0b000000)   0b1)) ;310
                            nl <= #1 4'h0; $display(";A 312");		//(assert (= nl    0h0)) ;312
                            play <= #1 1'b0; $display(";A 313");		//(assert (= play    0b0)) ;313
                            count2 = 6'b001000; $display(";A 314");		//(assert (= count2    0b001000)) ;314
                            gamma = 5'b10111; $display(";A 315");		//(assert (= gamma    0b10111)) ;315
                        end
                        else begin
                            $display(";A 311");		//(assert (= (bv-comp count2  0b000000)   0b0)) ;311
                            count2 = (count2 - 6'b000001); $display(";A 316");		//(assert (= count2    (bv-sub count2  0b000001))) ;316
                            gamma = 5'b10110; $display(";A 317");		//(assert (= gamma    0b10110)) ;317
                        end
                    end
                5'b10111 :
                    begin
                        $display(";A 318");		//(assert (= gamma    0b10111)) ;318
                        if ((count2 == 6'b000000)) begin
                            $display(";A 319");		//(assert (= (bv-comp count2  0b000000)   0b1)) ;319
                            nl[data_out] <= #1 1'sb1; $display(";A 321");		//(assert (= (bv-extract 0 0 nl )   0b1)) ;321
                            play <= #1 1'b1; $display(";A 322");		//(assert (= play    0b1)) ;322
                            sound <= #1 3'b101; $display(";A 323");		//(assert (= sound    0b101)) ;323
                            count2 = 6'b001000; $display(";A 324");		//(assert (= count2    0b001000)) ;324
                            gamma = 5'b10110; $display(";A 325");		//(assert (= gamma    0b10110)) ;325
                        end
                        else begin
                            $display(";A 320");		//(assert (= (bv-comp count2  0b000000)   0b0)) ;320
                            count2 = (count2 - 6'b000001); $display(";A 326");		//(assert (= count2    (bv-sub count2  0b000001))) ;326
                            gamma = 5'b10111; $display(";A 327");		//(assert (= gamma    0b10111)) ;327
                        end
                    end
                5'b11000 :
                    begin
                        $display(";A 328");		//(assert (= gamma    0b11000)) ;328
                        if ((count2 == 6'b000000)) begin
                            $display(";A 329");		//(assert (= (bv-comp count2  0b000000)   0b1)) ;329
                            nl <= #1 4'h1; $display(";A 331");		//(assert (= nl    0h1)) ;331
                            play <= #1 1'b0; $display(";A 332");		//(assert (= play    0b0)) ;332
                            count2 = 6'b001000; $display(";A 333");		//(assert (= count2    0b001000)) ;333
                            gamma = 5'b11001; $display(";A 334");		//(assert (= gamma    0b11001)) ;334
                        end
                        else begin
                            $display(";A 330");		//(assert (= (bv-comp count2  0b000000)   0b0)) ;330
                            count2 = (count2 - 6'b000001); $display(";A 335");		//(assert (= count2    (bv-sub count2  0b000001))) ;335
                            gamma = 5'b11000; $display(";A 336");		//(assert (= gamma    0b11000)) ;336
                        end
                    end
                5'b11001 :
                    begin
                        $display(";A 337");		//(assert (= gamma    0b11001)) ;337
                        if ((count2 == 6'b000000)) begin
                            $display(";A 338");		//(assert (= (bv-comp count2  0b000000)   0b1)) ;338
                            nl <= #1 4'h0; $display(";A 340");		//(assert (= nl    0h0)) ;340
                            play <= #1 1'b1; $display(";A 341");		//(assert (= play    0b1)) ;341
                            sound <= #1 3'b100; $display(";A 342");		//(assert (= sound    0b100)) ;342
                            count2 = 6'b001000; $display(";A 343");		//(assert (= count2    0b001000)) ;343
                            gamma = 5'b11000; $display(";A 344");		//(assert (= gamma    0b11000)) ;344
                        end
                        else begin
                            $display(";A 339");		//(assert (= (bv-comp count2  0b000000)   0b0)) ;339
                            count2 = (count2 - 6'b000001); $display(";A 345");		//(assert (= count2    (bv-sub count2  0b000001))) ;345
                            gamma = 5'b11001; $display(";A 346");		//(assert (= gamma    0b11001)) ;346
                        end
                    end
                default:
                    begin
                        $display(";A 347");		//(assert (= (and (/= gamma  0b00000) (/= gamma  0b00001) (/= gamma  0b00010) (/= gamma  0b00011) (/= gamma  0b00100) (/= gamma  0b00101) (/= gamma  0b00110) (/= gamma  0b00111) (/= gamma  0b01000) (/= gamma  0b01001) (/= gamma  0b01010) (/= gamma  0b01011) (/= gamma  0b01100) (/= gamma  0b01101) (/= gamma  0b01110) (/= gamma  0b01111) (/= gamma  0b10000) (/= gamma  0b10001) (/= gamma  0b10010) (/= gamma  0b10011) (/= gamma  0b10100) (/= gamma  0b10101) (/= gamma  0b10110) (/= gamma  0b10111) (/= gamma  0b11000) (/= gamma  0b11001))   true)) ;347
                        gamma = 5'b00001; $display(";A 348");		//(assert (= gamma    0b00001)) ;348
                    end
            endcase
        end
    end

    // Displaying module variables
    always @(posedge clock) begin
      $display(";R address = %b", address);
      $display(";R count = %b", count);
      $display(";R count2 = %b", count2);
      $display(";R counter = %b", counter);
      $display(";R data_in = %b", data_in);
      $display(";R data_out = %b", data_out);
      $display(";R gamma = %b", gamma);
      $display(";R ind = %b", ind);
      $display(";R max = %b", max);
      $display(";R memory[0] = %b", memory[0]);
      $display(";R memory[1] = %b", memory[1]);
      $display(";R memory[2] = %b", memory[2]);
      $display(";R memory[3] = %b", memory[3]);
      $display(";R memory[4] = %b", memory[4]);
      $display(";R memory[5] = %b", memory[5]);
      $display(";R memory[6] = %b", memory[6]);
      $display(";R memory[7] = %b", memory[7]);
      $display(";R memory[8] = %b", memory[8]);
      $display(";R memory[9] = %b", memory[9]);
      $display(";R memory[10] = %b", memory[10]);
      $display(";R memory[11] = %b", memory[11]);
      $display(";R memory[12] = %b", memory[12]);
      $display(";R memory[13] = %b", memory[13]);
      $display(";R memory[14] = %b", memory[14]);
      $display(";R memory[15] = %b", memory[15]);
      $display(";R memory[16] = %b", memory[16]);
      $display(";R memory[17] = %b", memory[17]);
      $display(";R memory[18] = %b", memory[18]);
      $display(";R memory[19] = %b", memory[19]);
      $display(";R memory[20] = %b", memory[20]);
      $display(";R memory[21] = %b", memory[21]);
      $display(";R memory[22] = %b", memory[22]);
      $display(";R memory[23] = %b", memory[23]);
      $display(";R memory[24] = %b", memory[24]);
      $display(";R memory[25] = %b", memory[25]);
      $display(";R memory[26] = %b", memory[26]);
      $display(";R memory[27] = %b", memory[27]);
      $display(";R memory[28] = %b", memory[28]);
      $display(";R memory[29] = %b", memory[29]);
      $display(";R memory[30] = %b", memory[30]);
      $display(";R memory[31] = %b", memory[31]);
      $display(";R nl = %b", nl);
      $display(";R nloss = %b", nloss);
      $display(";R num = %b", num);
      $display(";R play = %b", play);
      $display(";R s = %b", s);
      $display(";R sound = %b", sound);
      $display(";R speaker = %b", speaker);
      $display(";R timebase = %b", timebase);
      $display(";R wr = %b", wr);
    end
endmodule

