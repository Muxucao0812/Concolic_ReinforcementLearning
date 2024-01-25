// Following code segment is generated from /home/ziyue/researchlib/Micro_Eletronic/STSearch/tests/b15/src/b15.v:16
module b15(BE_n, Address, W_R_n, D_C_n, M_IO_n, ADS_n, Datai, Datao, CLOCK, NA_n, BS16_n, READY_n, HOLD, RESET,sig_display,__obs);
    output [3:0] BE_n;
    output [29:0] Address;
    output W_R_n;
    output D_C_n;
    output M_IO_n;
    output ADS_n;
    input [31:0] Datai;
    output Datao;
    input CLOCK;
    input NA_n;
    input BS16_n;
    input READY_n;
    input HOLD;
    input RESET;
    input sig_display;
    input __obs;

    reg ADS_n;
    reg [29:0] Address;
    reg [3:0] BE_n;
    reg [3:0] ByteEnable;
    reg CodeFetch;
    reg D_C_n;
    reg signed [31:0] DataWidth;
    reg signed [31:0] Datao;
    reg signed [31:0] EAX;
    reg signed [31:0] EBX;
    reg Extended;
    reg Flush;
    reg signed [31:0] InstAddrPointer;
    reg [7:0] InstQueue [15:0];
    reg [4:0] InstQueueRd_Addr;
    reg [4:0] InstQueueWr_Addr;
    reg M_IO_n;
    reg MemoryFetch;
    reg More;
    reg NonAligned;
    reg signed [31:0] PhyAddrPointer;
    reg ReadRequest;
    reg RequestPending;
    reg [2:0] State;
    reg [3:0] State2;
    reg StateBS16;
    reg StateNA;
    reg W_R_n;
    reg signed [31:0] fWord;
    reg [15:0] lWord;
    reg signed [31:0] rEIP;
    reg [14:0] uWord;

    // Following code segment is generated from /home/ziyue/researchlib/Micro_Eletronic/STSearch/tests/b15/src/b15.v:130
    always @(posedge RESET or posedge CLOCK) begin
        if ((RESET == 1'b1)) begin
            BE_n <= #1 4'b0000; $display(";A 2");		//(= BE_n    0b0000)) ;2
            Address <= #1 30'b000000000000000000000000000000; $display(";A 3");		//(= Address    0b000000000000000000000000000000)) ;3
            W_R_n <= #1 1'b0; $display(";A 4");		//(= W_R_n    0b0)) ;4
            D_C_n <= #1 1'b0; $display(";A 5");		//(= D_C_n    0b0)) ;5
            M_IO_n <= #1 1'b0; $display(";A 6");		//(= M_IO_n    0b0)) ;6
            ADS_n <= #1 1'b0; $display(";A 7");		//(= ADS_n    0b0)) ;7
            State <= #1 3'b000; $display(";A 8");		//(= State    0b000)) ;8
            StateNA <= #1 1'b0; $display(";A 9");		//(= StateNA    0b0)) ;9
            StateBS16 <= #1 1'b0; $display(";A 10");		//(= StateBS16    0b0)) ;10
            DataWidth <= #1 32'sb00000000000000000000000000000000; $display(";A 11");		//(= DataWidth    0b00000000000000000000000000000000)) ;11
        end
        else begin
            case (State)
                3'b000 :
                    begin
                        $display(";A 12");		//(= State    0b000)) ;12
                        D_C_n <= #1 1'b1; $display(";A 13");		//(= D_C_n    0b1)) ;13
                        ADS_n <= #1 1'b1; $display(";A 14");		//(= ADS_n    0b1)) ;14
                        State <= #1 3'b001; $display(";A 15");		//(= State    0b001)) ;15
                        StateNA <= #1 1'b1; $display(";A 16");		//(= StateNA    0b1)) ;16
                        StateBS16 <= #1 1'b1; $display(";A 17");		//(= StateBS16    0b1)) ;17
                        DataWidth <= #1 32'sb00000000000000000000000000000010; $display(";A 18");		//(= DataWidth    0b00000000000000000000000000000010)) ;18
                        State <= #1 3'b001; $display(";A 19");		//(= State    0b001)) ;19
                    end
                3'b001 :
                    begin
                        $display(";A 20");		//(= State    0b001)) ;20
                        if ((RequestPending == 1'b1)) begin
                            $display(";A 21");		//(= (bv-comp RequestPending  0b1)   0b1)) ;21
                            State <= #1 3'b010; $display(";A 23");		//(= State    0b010)) ;23
                        end
                        else begin
                            $display(";A 22");		//(= (bv-comp RequestPending  0b1)   0b0)) ;22
                            if ((HOLD == 1'b1)) begin
                                $display(";A 24");		//(= (bv-comp HOLD  0b1)   0b1)) ;24
                                State <= #1 3'b101; $display(";A 26");		//(= State    0b101)) ;26
                            end
                            else begin
                                $display(";A 25");		//(= (bv-comp HOLD  0b1)   0b0)) ;25
                                State <= #1 3'b001; $display(";A 27");		//(= State    0b001)) ;27
                            end
                        end
                    end
                3'b010 :
                    begin
                        $display(";A 28");		//(= State    0b010)) ;28
                        Address <= #1 ((rEIP / 32'sb00000000000000000000000000000100) % 32'sb01000000000000000000000000000000); $display(";A 29");		//(= Address    (bv-smod (bv-sdiv rEIP  0b00000000000000000000000000000100) 0b01000000000000000000000000000000))) ;29
                        BE_n <= #1 ByteEnable; $display(";A 30");		//(= BE_n    ByteEnable )) ;30
                        M_IO_n <= #1 MemoryFetch; $display(";A 31");		//(= M_IO_n    MemoryFetch )) ;31
                        if ((ReadRequest == 1'b1)) begin
                            $display(";A 32");		//(= (bv-comp ReadRequest  0b1)   0b1)) ;32
                            W_R_n <= #1 1'b0; $display(";A 34");		//(= W_R_n    0b0)) ;34
                        end
                        else begin
                            $display(";A 33");		//(= (bv-comp ReadRequest  0b1)   0b0)) ;33
                            W_R_n <= #1 1'b1; $display(";A 35");		//(= W_R_n    0b1)) ;35
                        end
                        if ((CodeFetch == 1'b1)) begin
                            $display(";A 36");		//(= (bv-comp CodeFetch  0b1)   0b1)) ;36
                            D_C_n <= #1 1'b0; $display(";A 38");		//(= D_C_n    0b0)) ;38
                        end
                        else begin
                            $display(";A 37");		//(= (bv-comp CodeFetch  0b1)   0b0)) ;37
                            D_C_n <= #1 1'b1; $display(";A 39");		//(= D_C_n    0b1)) ;39
                        end
                        ADS_n <= #1 1'b0; $display(";A 40");		//(= ADS_n    0b0)) ;40
                        State <= #1 3'b011; $display(";A 41");		//(= State    0b011)) ;41
                    end
                3'b011 :
                    begin
                        $display(";A 42");		//(= State    0b011)) ;42
                        if ((((READY_n == 1'b0) & (HOLD == 1'b0)) & (RequestPending == 1'b1))) begin
                            $display(";A 43");		//(= (bv-and (bv-and (bv-comp READY_n  0b0) (bv-comp HOLD  0b0)) (bv-comp RequestPending  0b1))   0b1)) ;43
                            State <= #1 3'b010; $display(";A 45");		//(= State    0b010)) ;45
                        end
                        else begin
                            $display(";A 44");		//(= (bv-and (bv-and (bv-comp READY_n  0b0) (bv-comp HOLD  0b0)) (bv-comp RequestPending  0b1))   0b0)) ;44
                            if (((READY_n == 1'b1) & (NA_n == 1'b1))) begin
                                $display(";A 46");		//(= (bv-and (bv-comp READY_n  0b1) (bv-comp NA_n  0b1))   0b1)) ;46
                            end
                            else begin
                                $display(";A 47");		//(= (bv-and (bv-comp READY_n  0b1) (bv-comp NA_n  0b1))   0b0)) ;47
                                if ((((RequestPending == 1'b1) | (HOLD == 1'b1)) & ((READY_n == 1'b1) & (NA_n == 1'b0)))) begin
                                    $display(";A 48");		//(= (bv-and (bv-or (bv-comp RequestPending  0b1) (bv-comp HOLD  0b1)) (bv-and (bv-comp READY_n  0b1) (bv-comp NA_n  0b0)))   0b1)) ;48
                                    State <= #1 3'b111; $display(";A 50");		//(= State    0b111)) ;50
                                end
                                else begin
                                    $display(";A 49");		//(= (bv-and (bv-or (bv-comp RequestPending  0b1) (bv-comp HOLD  0b1)) (bv-and (bv-comp READY_n  0b1) (bv-comp NA_n  0b0)))   0b0)) ;49
                                    if (((((RequestPending == 1'b1) & (HOLD == 1'b0)) & (READY_n == 1'b1)) & (NA_n == 1'b0))) begin
                                        $display(";A 51");		//(= (bv-and (bv-and (bv-and (bv-comp RequestPending  0b1) (bv-comp HOLD  0b0)) (bv-comp READY_n  0b1)) (bv-comp NA_n  0b0))   0b1)) ;51
                                        State <= #1 3'b110; $display(";A 53");		//(= State    0b110)) ;53
                                    end
                                    else begin
                                        $display(";A 52");		//(= (bv-and (bv-and (bv-and (bv-comp RequestPending  0b1) (bv-comp HOLD  0b0)) (bv-comp READY_n  0b1)) (bv-comp NA_n  0b0))   0b0)) ;52
                                        if ((((RequestPending == 1'b0) & (HOLD == 1'b0)) & (READY_n == 1'b0))) begin
                                            $display(";A 54");		//(= (bv-and (bv-and (bv-comp RequestPending  0b0) (bv-comp HOLD  0b0)) (bv-comp READY_n  0b0))   0b1)) ;54
                                            State <= #1 3'b001; $display(";A 56");		//(= State    0b001)) ;56
                                        end
                                        else begin
                                            $display(";A 55");		//(= (bv-and (bv-and (bv-comp RequestPending  0b0) (bv-comp HOLD  0b0)) (bv-comp READY_n  0b0))   0b0)) ;55
                                            if (((HOLD == 1'b1) & (READY_n == 1'b1))) begin
                                                $display(";A 57");		//(= (bv-and (bv-comp HOLD  0b1) (bv-comp READY_n  0b1))   0b1)) ;57
                                                State <= #1 3'b101; $display(";A 59");		//(= State    0b101)) ;59
                                            end
                                            else begin
                                                $display(";A 58");		//(= (bv-and (bv-comp HOLD  0b1) (bv-comp READY_n  0b1))   0b0)) ;58
                                                State <= #1 3'b011; $display(";A 60");		//(= State    0b011)) ;60
                                            end
                                        end
                                    end
                                end
                            end
                        end
                        StateBS16 <= #1 BS16_n; $display(";A 61");		//(= StateBS16    BS16_n )) ;61
                        if ((BS16_n == 1'b0)) begin
                            $display(";A 62");		//(= (bv-comp BS16_n  0b0)   0b1)) ;62
                            DataWidth <= #1 32'sb00000000000000000000000000000001; $display(";A 64");		//(= DataWidth    0b00000000000000000000000000000001)) ;64
                        end
                        else begin
                            $display(";A 63");		//(= (bv-comp BS16_n  0b0)   0b0)) ;63
                            DataWidth <= #1 32'sb00000000000000000000000000000010; $display(";A 65");		//(= DataWidth    0b00000000000000000000000000000010)) ;65
                        end
                        StateNA <= #1 NA_n; $display(";A 66");		//(= StateNA    NA_n )) ;66
                        ADS_n <= #1 1'b1; $display(";A 67");		//(= ADS_n    0b1)) ;67
                    end
                3'b100 :
                    begin
                        $display(";A 68");		//(= State    0b100)) ;68
                        if ((((NA_n == 1'b0) & (HOLD == 1'b0)) & (RequestPending == 1'b1))) begin
                            $display(";A 69");		//(= (bv-and (bv-and (bv-comp NA_n  0b0) (bv-comp HOLD  0b0)) (bv-comp RequestPending  0b1))   0b1)) ;69
                            State <= #1 3'b110; $display(";A 71");		//(= State    0b110)) ;71
                        end
                        else begin
                            $display(";A 70");		//(= (bv-and (bv-and (bv-comp NA_n  0b0) (bv-comp HOLD  0b0)) (bv-comp RequestPending  0b1))   0b0)) ;70
                            if (((NA_n == 1'b0) & ((HOLD == 1'b1) | (RequestPending == 1'b0)))) begin
                                $display(";A 72");		//(= (bv-and (bv-comp NA_n  0b0) (bv-or (bv-comp HOLD  0b1) (bv-comp RequestPending  0b0)))   0b1)) ;72
                                State <= #1 3'b111; $display(";A 74");		//(= State    0b111)) ;74
                            end
                            else begin
                                $display(";A 73");		//(= (bv-and (bv-comp NA_n  0b0) (bv-or (bv-comp HOLD  0b1) (bv-comp RequestPending  0b0)))   0b0)) ;73
                                if ((NA_n == 1'b1)) begin
                                    $display(";A 75");		//(= (bv-comp NA_n  0b1)   0b1)) ;75
                                    State <= #1 3'b011; $display(";A 77");		//(= State    0b011)) ;77
                                end
                                else begin
                                    $display(";A 76");		//(= (bv-comp NA_n  0b1)   0b0)) ;76
                                    State <= #1 3'b100; $display(";A 78");		//(= State    0b100)) ;78
                                end
                            end
                        end
                        StateBS16 <= #1 BS16_n; $display(";A 79");		//(= StateBS16    BS16_n )) ;79
                        if ((BS16_n == 1'b0)) begin
                            $display(";A 80");		//(= (bv-comp BS16_n  0b0)   0b1)) ;80
                            DataWidth <= #1 32'sb00000000000000000000000000000001; $display(";A 82");		//(= DataWidth    0b00000000000000000000000000000001)) ;82
                        end
                        else begin
                            $display(";A 81");		//(= (bv-comp BS16_n  0b0)   0b0)) ;81
                            DataWidth <= #1 32'sb00000000000000000000000000000010; $display(";A 83");		//(= DataWidth    0b00000000000000000000000000000010)) ;83
                        end
                        StateNA <= #1 NA_n; $display(";A 84");		//(= StateNA    NA_n )) ;84
                        ADS_n <= #1 1'b1; $display(";A 85");		//(= ADS_n    0b1)) ;85
                    end
                3'b101 :
                    begin
                        $display(";A 86");		//(= State    0b101)) ;86
                        if (((HOLD == 1'b0) & (RequestPending == 1'b1))) begin
                            $display(";A 87");		//(= (bv-and (bv-comp HOLD  0b0) (bv-comp RequestPending  0b1))   0b1)) ;87
                            State <= #1 3'b010; $display(";A 89");		//(= State    0b010)) ;89
                        end
                        else begin
                            $display(";A 88");		//(= (bv-and (bv-comp HOLD  0b0) (bv-comp RequestPending  0b1))   0b0)) ;88
                            if (((HOLD == 1'b0) & (RequestPending == 1'b0))) begin
                                $display(";A 90");		//(= (bv-and (bv-comp HOLD  0b0) (bv-comp RequestPending  0b0))   0b1)) ;90
                                State <= #1 3'b001; $display(";A 92");		//(= State    0b001)) ;92
                            end
                            else begin
                                $display(";A 91");		//(= (bv-and (bv-comp HOLD  0b0) (bv-comp RequestPending  0b0))   0b0)) ;91
                                State <= #1 3'b101; $display(";A 93");		//(= State    0b101)) ;93
                            end
                        end
                    end
                3'b110 :
                    begin
                        $display(";A 94");		//(= State    0b110)) ;94
                        Address <= #1 ((rEIP / 32'sb00000000000000000000000000000010) % 32'sb01000000000000000000000000000000); $display(";A 95");		//(= Address    (bv-smod (bv-sdiv rEIP  0b00000000000000000000000000000010) 0b01000000000000000000000000000000))) ;95
                        BE_n <= #1 ByteEnable; $display(";A 96");		//(= BE_n    ByteEnable )) ;96
                        M_IO_n <= #1 MemoryFetch; $display(";A 97");		//(= M_IO_n    MemoryFetch )) ;97
                        if ((ReadRequest == 1'b1)) begin
                            $display(";A 98");		//(= (bv-comp ReadRequest  0b1)   0b1)) ;98
                            W_R_n <= #1 1'b0; $display(";A 100");		//(= W_R_n    0b0)) ;100
                        end
                        else begin
                            $display(";A 99");		//(= (bv-comp ReadRequest  0b1)   0b0)) ;99
                            W_R_n <= #1 1'b1; $display(";A 101");		//(= W_R_n    0b1)) ;101
                        end
                        if ((CodeFetch == 1'b1)) begin
                            $display(";A 102");		//(= (bv-comp CodeFetch  0b1)   0b1)) ;102
                            D_C_n <= #1 1'b0; $display(";A 104");		//(= D_C_n    0b0)) ;104
                        end
                        else begin
                            $display(";A 103");		//(= (bv-comp CodeFetch  0b1)   0b0)) ;103
                            D_C_n <= #1 1'b1; $display(";A 105");		//(= D_C_n    0b1)) ;105
                        end
                        ADS_n <= #1 1'b0; $display(";A 106");		//(= ADS_n    0b0)) ;106
                        if ((READY_n == 1'b0)) begin
                            $display(";A 107");		//(= (bv-comp READY_n  0b0)   0b1)) ;107
                            State <= #1 3'b100; $display(";A 109");		//(= State    0b100)) ;109
                        end
                        else begin
                            $display(";A 108");		//(= (bv-comp READY_n  0b0)   0b0)) ;108
                            State <= #1 3'b110; $display(";A 110");		//(= State    0b110)) ;110
                        end
                    end
                3'b111 :
                    begin
                        $display(";A 111");		//(= State    0b111)) ;111
                        if ((((READY_n == 1'b1) & (RequestPending == 1'b1)) & (HOLD == 1'b0))) begin
                            $display(";A 112");		//(= (bv-and (bv-and (bv-comp READY_n  0b1) (bv-comp RequestPending  0b1)) (bv-comp HOLD  0b0))   0b1)) ;112
                            State <= #1 3'b110; $display(";A 114");		//(= State    0b110)) ;114
                        end
                        else begin
                            $display(";A 113");		//(= (bv-and (bv-and (bv-comp READY_n  0b1) (bv-comp RequestPending  0b1)) (bv-comp HOLD  0b0))   0b0)) ;113
                            if (((READY_n == 1'b0) & (HOLD == 1'b1))) begin
                                $display(";A 115");		//(= (bv-and (bv-comp READY_n  0b0) (bv-comp HOLD  0b1))   0b1)) ;115
                                State <= #1 3'b101; $display(";A 117");		//(= State    0b101)) ;117
                            end
                            else begin
                                $display(";A 116");		//(= (bv-and (bv-comp READY_n  0b0) (bv-comp HOLD  0b1))   0b0)) ;116
                                if ((((READY_n == 1'b0) & (HOLD == 1'b0)) & (RequestPending == 1'b1))) begin
                                    $display(";A 118");		//(= (bv-and (bv-and (bv-comp READY_n  0b0) (bv-comp HOLD  0b0)) (bv-comp RequestPending  0b1))   0b1)) ;118
                                    State <= #1 3'b010; $display(";A 120");		//(= State    0b010)) ;120
                                end
                                else begin
                                    $display(";A 119");		//(= (bv-and (bv-and (bv-comp READY_n  0b0) (bv-comp HOLD  0b0)) (bv-comp RequestPending  0b1))   0b0)) ;119
                                    if ((((READY_n == 1'b0) & (HOLD == 1'b0)) & (RequestPending == 1'b0))) begin
                                        $display(";A 121");		//(= (bv-and (bv-and (bv-comp READY_n  0b0) (bv-comp HOLD  0b0)) (bv-comp RequestPending  0b0))   0b1)) ;121
                                        State <= #1 3'b001; $display(";A 123");		//(= State    0b001)) ;123
                                    end
                                    else begin
                                        $display(";A 122");		//(= (bv-and (bv-and (bv-comp READY_n  0b0) (bv-comp HOLD  0b0)) (bv-comp RequestPending  0b0))   0b0)) ;122
                                        State <= #1 3'b111; $display(";A 124");		//(= State    0b111)) ;124
                                    end
                                end
                            end
                        end
                    end
            endcase
        end
    if (sig_display) begin
        $display(";F ADS_n = %b;Address = %b;BE_n = %b;ByteEnable = %b;CodeFetch = %b;D_C_n = %b;DataWidth = %b;Datao = %b;EAX = %b;EBX = %b;Extended = %b;Flush = %b;InstAddrPointer = %b;InstQueue = %b;InstQueueRd_Addr = %b;InstQueueWr_Addr = %b;M_IO_n = %b;MemoryFetch = %b;More = %b;NonAligned = %b;PhyAddrPointer = %b;ReadRequest = %b;RequestPending = %b;State = %b;State2 = %b;StateBS16 = %b;StateNA = %b;W_R_n = %b;fWord = %b;lWord = %b;rEIP = %b;uWord = %b;",ADS_n,Address,BE_n,ByteEnable,CodeFetch,D_C_n,DataWidth,Datao,EAX,EBX,Extended,Flush,InstAddrPointer,InstQueue,InstQueueRd_Addr,InstQueueWr_Addr,M_IO_n,MemoryFetch,More,NonAligned,PhyAddrPointer,ReadRequest,RequestPending,State,State2,StateBS16,StateNA,W_R_n,fWord,lWord,rEIP,uWord);
        end
    end

    // Following code segment is generated from /home/ziyue/researchlib/Micro_Eletronic/STSearch/tests/b15/src/b15.v:271
    always @(posedge RESET or posedge CLOCK) begin
        if ((RESET == 1'b1)) begin
            State2 = 4'b0000; $display(";A 127");		//(= State2    0b0000)) ;127
            InstQueue[4'b0000] = 8'b00000000; $display(";A 128");		//(= InstQueue 0    0b00000000)) ;128
            InstQueue[4'b0001] = 8'b00000000; $display(";A 129");		//(= InstQueue 1    0b00000000)) ;129
            InstQueue[4'b0010] = 8'b00000000; $display(";A 130");		//(= InstQueue 2    0b00000000)) ;130
            InstQueue[4'b0011] = 8'b00000000; $display(";A 131");		//(= InstQueue 3    0b00000000)) ;131
            InstQueue[4'b0100] = 8'b00000000; $display(";A 132");		//(= InstQueue 4    0b00000000)) ;132
            InstQueue[4'b0101] = 8'b00000000; $display(";A 133");		//(= InstQueue 5    0b00000000)) ;133
            InstQueue[4'b0110] = 8'b00000000; $display(";A 134");		//(= InstQueue 6    0b00000000)) ;134
            InstQueue[4'b0111] = 8'b00000000; $display(";A 135");		//(= InstQueue 7    0b00000000)) ;135
            InstQueue[4'b1000] = 8'b00000000; $display(";A 136");		//(= InstQueue 8    0b00000000)) ;136
            InstQueue[4'b1001] = 8'b00000000; $display(";A 137");		//(= InstQueue 9    0b00000000)) ;137
            InstQueue[4'b1010] = 8'b00000000; $display(";A 138");		//(= InstQueue 10    0b00000000)) ;138
            InstQueue[4'b1011] = 8'b00000000; $display(";A 139");		//(= InstQueue 11    0b00000000)) ;139
            InstQueue[4'b1100] = 8'b00000000; $display(";A 140");		//(= InstQueue 12    0b00000000)) ;140
            InstQueue[4'b1101] = 8'b00000000; $display(";A 141");		//(= InstQueue 13    0b00000000)) ;141
            InstQueue[4'b1110] = 8'b00000000; $display(";A 142");		//(= InstQueue 14    0b00000000)) ;142
            InstQueue[4'b1111] = 8'b00000000; $display(";A 143");		//(= InstQueue 15    0b00000000)) ;143
            InstQueueRd_Addr = 5'b00000; $display(";A 144");		//(= InstQueueRd_Addr    0b00000)) ;144
            InstQueueWr_Addr = 5'b00000; $display(";A 145");		//(= InstQueueWr_Addr    0b00000)) ;145
            InstAddrPointer = 32'sb00000000000000000000000000000000; $display(";A 146");		//(= InstAddrPointer    0b00000000000000000000000000000000)) ;146
            PhyAddrPointer = 32'sb00000000000000000000000000000000; $display(";A 147");		//(= PhyAddrPointer    0b00000000000000000000000000000000)) ;147
            Extended = 1'b0; $display(";A 148");		//(= Extended    0b0)) ;148
            More = 1'b0; $display(";A 149");		//(= More    0b0)) ;149
            Flush = 1'b0; $display(";A 150");		//(= Flush    0b0)) ;150
            lWord = 16'b0000000000000000; $display(";A 151");		//(= lWord    0b0000000000000000)) ;151
            uWord = 15'b000000000000000; $display(";A 152");		//(= uWord    0b000000000000000)) ;152
            fWord = 32'sb00000000000000000000000000000000; $display(";A 153");		//(= fWord    0b00000000000000000000000000000000)) ;153
            CodeFetch <= #1 1'b0; $display(";A 154");		//(= CodeFetch    0b0)) ;154
            Datao <= #1 32'sb00000000000000000000000000000000; $display(";A 155");		//(= Datao    0b00000000000000000000000000000000)) ;155
            EAX <= #1 32'sb00000000000000000000000000000000; $display(";A 156");		//(= EAX    0b00000000000000000000000000000000)) ;156
            EBX <= #1 32'sb00000000000000000000000000000000; $display(";A 157");		//(= EBX    0b00000000000000000000000000000000)) ;157
            rEIP <= #1 32'sb00000000000000000000000000000000; $display(";A 158");		//(= rEIP    0b00000000000000000000000000000000)) ;158
            ReadRequest <= #1 1'b0; $display(";A 159");		//(= ReadRequest    0b0)) ;159
            MemoryFetch <= #1 1'b0; $display(";A 160");		//(= MemoryFetch    0b0)) ;160
            RequestPending <= #1 1'b0; $display(";A 161");		//(= RequestPending    0b0)) ;161
        end
        else begin
            case (State2)
                4'b0000 :
                    begin
                        $display(";A 162");		//(= State2    0b0000)) ;162
                        PhyAddrPointer = rEIP; $display(";A 163");		//(= PhyAddrPointer    rEIP )) ;163
                        InstAddrPointer = PhyAddrPointer; $display(";A 164");		//(= InstAddrPointer    PhyAddrPointer )) ;164
                        State2 = 4'b0001; $display(";A 165");		//(= State2    0b0001)) ;165
                        rEIP <= #1 32'sb00000000000011111111111111110000; $display(";A 166");		//(= rEIP    0b00000000000011111111111111110000)) ;166
                        ReadRequest <= #1 1'b1; $display(";A 167");		//(= ReadRequest    0b1)) ;167
                        MemoryFetch <= #1 1'b1; $display(";A 168");		//(= MemoryFetch    0b1)) ;168
                        RequestPending <= #1 1'b1; $display(";A 169");		//(= RequestPending    0b1)) ;169
                    end
                4'b0001 :
                    begin
                        $display(";A 170");		//(= State2    0b0001)) ;170
                        RequestPending <= #1 1'b1; $display(";A 171");		//(= RequestPending    0b1)) ;171
                        ReadRequest <= #1 1'b1; $display(";A 172");		//(= ReadRequest    0b1)) ;172
                        MemoryFetch <= #1 1'b1; $display(";A 173");		//(= MemoryFetch    0b1)) ;173
                        CodeFetch <= #1 1'b1; $display(";A 174");		//(= CodeFetch    0b1)) ;174
                        if ((READY_n == 1'b0)) begin
                            $display(";A 175");		//(= (bv-comp READY_n  0b0)   0b1)) ;175
                            State2 = 4'b0010; $display(";A 177");		//(= State2    0b0010)) ;177
                        end
                        else begin
                            $display(";A 176");		//(= (bv-comp READY_n  0b0)   0b0)) ;176
                            State2 = 4'b0001; $display(";A 178");		//(= State2    0b0001)) ;178
                        end
                    end
                4'b0010 :
                    begin
                        $display(";A 179");		//(= State2    0b0010)) ;179
                        RequestPending <= #1 1'b0; $display(";A 180");		//(= RequestPending    0b0)) ;180
                        InstQueue[InstQueueWr_Addr] = (Datai % 32'b00000000000000000000000100000000); $display(";A 181");		//(= ( InstQueue InstQueueWr_Addr )   (bv-smod Datai  0b00000000000000000000000100000000))) ;181
                        InstQueueWr_Addr = ((InstQueueWr_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 182");		//(= InstQueueWr_Addr    (bv-smod (bv-add InstQueueWr_Addr  0b00001) 0b00000000000000000000000000010000))) ;182
                        InstQueue[InstQueueWr_Addr] = (Datai % 32'b00000000000000000000000100000000); $display(";A 183");		//(= ( InstQueue InstQueueWr_Addr )   (bv-smod Datai  0b00000000000000000000000100000000))) ;183
                        InstQueueWr_Addr = ((InstQueueWr_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 184");		//(= InstQueueWr_Addr    (bv-smod (bv-add InstQueueWr_Addr  0b00001) 0b00000000000000000000000000010000))) ;184
                        if ((StateBS16 == 1'b1)) begin
                            $display(";A 185");		//(= (bv-comp StateBS16  0b1)   0b1)) ;185
                            InstQueue[InstQueueWr_Addr] = ((Datai / 32'b00000000000000010000000000000000) % 32'b00000000000000000000000100000000); $display(";A 187");		//(= ( InstQueue InstQueueWr_Addr )   (bv-smod (bv-sdiv Datai  0b00000000000000010000000000000000) 0b00000000000000000000000100000000))) ;187
                            InstQueueWr_Addr = ((InstQueueWr_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 188");		//(= InstQueueWr_Addr    (bv-smod (bv-add InstQueueWr_Addr  0b00001) 0b00000000000000000000000000010000))) ;188
                            InstQueue[InstQueueWr_Addr] = ((Datai / 32'b00000001000000000000000000000000) % 32'b00000000000000000000000100000000); $display(";A 189");		//(= ( InstQueue InstQueueWr_Addr )   (bv-smod (bv-sdiv Datai  0b00000001000000000000000000000000) 0b00000000000000000000000100000000))) ;189
                            InstQueueWr_Addr = ((InstQueueWr_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 190");		//(= InstQueueWr_Addr    (bv-smod (bv-add InstQueueWr_Addr  0b00001) 0b00000000000000000000000000010000))) ;190
                            PhyAddrPointer = (PhyAddrPointer + 32'sb00000000000000000000000000000100); $display(";A 191");		//(= PhyAddrPointer    (bv-add PhyAddrPointer  0b00000000000000000000000000000100))) ;191
                            State2 = 4'b0101; $display(";A 192");		//(= State2    0b0101)) ;192
                        end
                        else begin
                            $display(";A 186");		//(= (bv-comp StateBS16  0b1)   0b0)) ;186
                            PhyAddrPointer = (PhyAddrPointer + 32'sb00000000000000000000000000000010); $display(";A 193");		//(= PhyAddrPointer    (bv-add PhyAddrPointer  0b00000000000000000000000000000010))) ;193
                            if ((PhyAddrPointer < 32'sb00000000000000000000000000000000)) begin
                                $display(";A 194");		//(= (bool-to-bv (bv-slt PhyAddrPointer  0b00000000000000000000000000000000))   0b1)) ;194
                                rEIP <= #1 (-PhyAddrPointer); $display(";A 196");		//(= rEIP    (bv-neg PhyAddrPointer ))) ;196
                            end
                            else begin
                                $display(";A 195");		//(= (bool-to-bv (bv-slt PhyAddrPointer  0b00000000000000000000000000000000))   0b0)) ;195
                                rEIP <= #1 PhyAddrPointer; $display(";A 197");		//(= rEIP    PhyAddrPointer )) ;197
                            end
                            State2 = 4'b0011; $display(";A 198");		//(= State2    0b0011)) ;198
                        end
                    end
                4'b0011 :
                    begin
                        $display(";A 199");		//(= State2    0b0011)) ;199
                        RequestPending <= #1 1'b1; $display(";A 200");		//(= RequestPending    0b1)) ;200
                        if ((READY_n == 1'b0)) begin
                            $display(";A 201");		//(= (bv-comp READY_n  0b0)   0b1)) ;201
                            State2 = 4'b0100; $display(";A 203");		//(= State2    0b0100)) ;203
                        end
                        else begin
                            $display(";A 202");		//(= (bv-comp READY_n  0b0)   0b0)) ;202
                            State2 = 4'b0011; $display(";A 204");		//(= State2    0b0011)) ;204
                        end
                    end
                4'b0100 :
                    begin
                        $display(";A 205");		//(= State2    0b0100)) ;205
                        RequestPending <= #1 1'b0; $display(";A 206");		//(= RequestPending    0b0)) ;206
                        InstQueue[InstQueueWr_Addr] = (Datai % 32'b00000000000000000000000100000000); $display(";A 207");		//(= ( InstQueue InstQueueWr_Addr )   (bv-smod Datai  0b00000000000000000000000100000000))) ;207
                        InstQueueWr_Addr = ((InstQueueWr_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 208");		//(= InstQueueWr_Addr    (bv-smod (bv-add InstQueueWr_Addr  0b00001) 0b00000000000000000000000000010000))) ;208
                        InstQueue[InstQueueWr_Addr] = (Datai % 32'b00000000000000000000000100000000); $display(";A 209");		//(= ( InstQueue InstQueueWr_Addr )   (bv-smod Datai  0b00000000000000000000000100000000))) ;209
                        InstQueueWr_Addr = ((InstQueueWr_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 210");		//(= InstQueueWr_Addr    (bv-smod (bv-add InstQueueWr_Addr  0b00001) 0b00000000000000000000000000010000))) ;210
                        PhyAddrPointer = (PhyAddrPointer + 32'sb00000000000000000000000000000010); $display(";A 211");		//(= PhyAddrPointer    (bv-add PhyAddrPointer  0b00000000000000000000000000000010))) ;211
                        State2 = 4'b0101; $display(";A 212");		//(= State2    0b0101)) ;212
                    end
                4'b0101 :
                    begin
                        $display(";A 213");		//(= State2    0b0101)) ;213
                        case (InstQueue[InstQueueRd_Addr])
                            8'b10010000 :
                                begin
                                    $display(";A 214");		//(= ( InstQueue InstQueueRd_Addr )   0b10010000)) ;214
                                    InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000001); $display(";A 215");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000001))) ;215
                                    InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 216");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00001) 0b00000000000000000000000000010000))) ;216
                                    Flush = 1'b0; $display(";A 217");		//(= Flush    0b0)) ;217
                                    More = 1'b0; $display(";A 218");		//(= More    0b0)) ;218
                                end
                            8'b01100110 :
                                begin
                                    $display(";A 219");		//(= ( InstQueue InstQueueRd_Addr )   0b01100110)) ;219
                                    InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000001); $display(";A 220");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000001))) ;220
                                    InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 221");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00001) 0b00000000000000000000000000010000))) ;221
                                    Extended = 1'b1; $display(";A 222");		//(= Extended    0b1)) ;222
                                    Flush = 1'b0; $display(";A 223");		//(= Flush    0b0)) ;223
                                    More = 1'b0; $display(";A 224");		//(= More    0b0)) ;224
                                end
                            8'b11101011 :
                                begin
                                    $display(";A 225");		//(= ( InstQueue InstQueueRd_Addr )   0b11101011)) ;225
                                    if (((InstQueueWr_Addr - InstQueueRd_Addr) >= 32'b00000000000000000000000000000011)) begin
                                        $display(";A 226");		//(= (bool-to-bv (bv-ge (bv-sub InstQueueWr_Addr  InstQueueRd_Addr ) 0b00000000000000000000000000000011))   0b1)) ;226
                                        if ((InstQueue[((InstQueueRd_Addr + 5'b00001) % 32'b00000000000000000000000000010000)] > 32'b00000000000000000000000001111111)) begin
                                            $display(";A 228");		//(= (bool-to-bv (bv-gt InstQueue 0  0b00000000000000000000000001111111))   0b1)) ;228
                                            PhyAddrPointer = ((InstAddrPointer + 32'b00000000000000000000000000000001) - (32'b00000000000000000000000011111111 - InstQueue[((InstQueueRd_Addr + 5'b00001) % 32'b00000000000000000000000000010000)])); $display(";A 230");		//(= PhyAddrPointer    (bv-sub (bv-add InstAddrPointer  0b00000000000000000000000000000001) (bv-sub 0b00000000000000000000000011111111 InstQueue 0 )))) ;230
                                            InstAddrPointer = PhyAddrPointer; $display(";A 231");		//(= InstAddrPointer    PhyAddrPointer )) ;231
                                        end
                                        else begin
                                            $display(";A 229");		//(= (bool-to-bv (bv-gt InstQueue 0  0b00000000000000000000000001111111))   0b0)) ;229
                                            PhyAddrPointer = ((InstAddrPointer + 32'b00000000000000000000000000000010) + InstQueue[((InstQueueRd_Addr + 5'b00001) % 32'b00000000000000000000000000010000)]); $display(";A 232");		//(= PhyAddrPointer    (bv-add (bv-add InstAddrPointer  0b00000000000000000000000000000010) InstQueue 0 ))) ;232
                                            InstAddrPointer = PhyAddrPointer; $display(";A 233");		//(= InstAddrPointer    PhyAddrPointer )) ;233
                                        end
                                        Flush = 1'b1; $display(";A 234");		//(= Flush    0b1)) ;234
                                        More = 1'b0; $display(";A 235");		//(= More    0b0)) ;235
                                    end
                                    else begin
                                        $display(";A 227");		//(= (bool-to-bv (bv-ge (bv-sub InstQueueWr_Addr  InstQueueRd_Addr ) 0b00000000000000000000000000000011))   0b0)) ;227
                                        Flush = 1'b0; $display(";A 236");		//(= Flush    0b0)) ;236
                                        More = 1'b1; $display(";A 237");		//(= More    0b1)) ;237
                                    end
                                end
                            8'b11101001 :
                                begin
                                    $display(";A 238");		//(= ( InstQueue InstQueueRd_Addr )   0b11101001)) ;238
                                    if (((InstQueueWr_Addr - InstQueueRd_Addr) >= 32'b00000000000000000000000000000101)) begin
                                        $display(";A 239");		//(= (bool-to-bv (bv-ge (bv-sub InstQueueWr_Addr  InstQueueRd_Addr ) 0b00000000000000000000000000000101))   0b1)) ;239
                                        PhyAddrPointer = ((InstAddrPointer + 32'b00000000000000000000000000000101) + InstQueue[((InstQueueRd_Addr + 5'b00001) % 32'b00000000000000000000000000010000)]); $display(";A 241");		//(= PhyAddrPointer    (bv-add (bv-add InstAddrPointer  0b00000000000000000000000000000101) InstQueue 0 ))) ;241
                                        InstAddrPointer = PhyAddrPointer; $display(";A 242");		//(= InstAddrPointer    PhyAddrPointer )) ;242
                                        Flush = 1'b1; $display(";A 243");		//(= Flush    0b1)) ;243
                                        More = 1'b0; $display(";A 244");		//(= More    0b0)) ;244
                                    end
                                    else begin
                                        $display(";A 240");		//(= (bool-to-bv (bv-ge (bv-sub InstQueueWr_Addr  InstQueueRd_Addr ) 0b00000000000000000000000000000101))   0b0)) ;240
                                        Flush = 1'b0; $display(";A 245");		//(= Flush    0b0)) ;245
                                        More = 1'b1; $display(";A 246");		//(= More    0b1)) ;246
                                    end
                                end
                            8'b11101010 :
                                begin
                                    $display(";A 247");		//(= ( InstQueue InstQueueRd_Addr )   0b11101010)) ;247
                                    InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000001); $display(";A 248");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000001))) ;248
                                    InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 249");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00001) 0b00000000000000000000000000010000))) ;249
                                    Flush = 1'b0; $display(";A 250");		//(= Flush    0b0)) ;250
                                    More = 1'b0; $display(";A 251");		//(= More    0b0)) ;251
                                end
                            8'b10110000 :
                                begin
                                    $display(";A 252");		//(= ( InstQueue InstQueueRd_Addr )   0b10110000)) ;252
                                    InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000001); $display(";A 253");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000001))) ;253
                                    InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 254");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00001) 0b00000000000000000000000000010000))) ;254
                                    Flush = 1'b0; $display(";A 255");		//(= Flush    0b0)) ;255
                                    More = 1'b0; $display(";A 256");		//(= More    0b0)) ;256
                                end
                            8'b10111000 :
                                begin
                                    $display(";A 257");		//(= ( InstQueue InstQueueRd_Addr )   0b10111000)) ;257
                                    if (((InstQueueWr_Addr - InstQueueRd_Addr) >= 32'b00000000000000000000000000000101)) begin
                                        $display(";A 258");		//(= (bool-to-bv (bv-ge (bv-sub InstQueueWr_Addr  InstQueueRd_Addr ) 0b00000000000000000000000000000101))   0b1)) ;258
                                        EAX <= #1 ((((InstQueue[((InstQueueRd_Addr + 5'b00100) % 32'b00000000000000000000000000010000)] * 32'b00000000100000000000000000000000) + (InstQueue[((InstQueueRd_Addr + 5'b00011) % 32'b00000000000000000000000000010000)] * 32'b00000000000000010000000000000000)) + (InstQueue[((InstQueueRd_Addr + 5'b00010) % 32'b00000000000000000000000000010000)] * 32'b00000000000000000000000100000000)) + InstQueue[((InstQueueRd_Addr + 5'b00001) % 32'b00000000000000000000000000010000)]); $display(";A 260");		//(= EAX    (bv-add (bv-add (bv-add (bv-mul InstQueue 0  0b00000000100000000000000000000000) (bv-mul InstQueue 0  0b00000000000000010000000000000000)) (bv-mul InstQueue 0  0b00000000000000000000000100000000)) InstQueue 0 ))) ;260
                                        More = 1'b0; $display(";A 261");		//(= More    0b0)) ;261
                                        Flush = 1'b0; $display(";A 262");		//(= Flush    0b0)) ;262
                                        InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000101); $display(";A 263");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000101))) ;263
                                        InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00101) % 32'b00000000000000000000000000010000); $display(";A 264");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00101) 0b00000000000000000000000000010000))) ;264
                                    end
                                    else begin
                                        $display(";A 259");		//(= (bool-to-bv (bv-ge (bv-sub InstQueueWr_Addr  InstQueueRd_Addr ) 0b00000000000000000000000000000101))   0b0)) ;259
                                        Flush = 1'b0; $display(";A 265");		//(= Flush    0b0)) ;265
                                        More = 1'b1; $display(";A 266");		//(= More    0b1)) ;266
                                    end
                                end
                            8'b10111011 :
                                begin
                                    $display(";A 267");		//(= ( InstQueue InstQueueRd_Addr )   0b10111011)) ;267
                                    if (((InstQueueWr_Addr - InstQueueRd_Addr) >= 32'b00000000000000000000000000000101)) begin
                                        $display(";A 268");		//(= (bool-to-bv (bv-ge (bv-sub InstQueueWr_Addr  InstQueueRd_Addr ) 0b00000000000000000000000000000101))   0b1)) ;268
                                        EBX <= #1 ((((InstQueue[((InstQueueRd_Addr + 5'b00100) % 32'b00000000000000000000000000010000)] * 32'b00000000100000000000000000000000) + (InstQueue[((InstQueueRd_Addr + 5'b00011) % 32'b00000000000000000000000000010000)] * 32'b00000000000000010000000000000000)) + (InstQueue[((InstQueueRd_Addr + 5'b00010) % 32'b00000000000000000000000000010000)] * 32'b00000000000000000000000100000000)) + InstQueue[((InstQueueRd_Addr + 5'b00001) % 32'b00000000000000000000000000000001)]); $display(";A 270");		//(= EBX    (bv-add (bv-add (bv-add (bv-mul InstQueue 0  0b00000000100000000000000000000000) (bv-mul InstQueue 0  0b00000000000000010000000000000000)) (bv-mul InstQueue 0  0b00000000000000000000000100000000)) InstQueue 0 ))) ;270
                                        More = 1'b0; $display(";A 271");		//(= More    0b0)) ;271
                                        Flush = 1'b0; $display(";A 272");		//(= Flush    0b0)) ;272
                                        InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000101); $display(";A 273");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000101))) ;273
                                        InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00101) % 32'b00000000000000000000000000010000); $display(";A 274");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00101) 0b00000000000000000000000000010000))) ;274
                                    end
                                    else begin
                                        $display(";A 269");		//(= (bool-to-bv (bv-ge (bv-sub InstQueueWr_Addr  InstQueueRd_Addr ) 0b00000000000000000000000000000101))   0b0)) ;269
                                        Flush = 1'b0; $display(";A 275");		//(= Flush    0b0)) ;275
                                        More = 1'b1; $display(";A 276");		//(= More    0b1)) ;276
                                    end
                                end
                            8'b10001011 :
                                begin
                                    $display(";A 277");		//(= ( InstQueue InstQueueRd_Addr )   0b10001011)) ;277
                                    if (((InstQueueWr_Addr - InstQueueRd_Addr) >= 32'b00000000000000000000000000000010)) begin
                                        $display(";A 278");		//(= (bool-to-bv (bv-ge (bv-sub InstQueueWr_Addr  InstQueueRd_Addr ) 0b00000000000000000000000000000010))   0b1)) ;278
                                        if ((EBX < 32'sb00000000000000000000000000000000)) begin
                                            $display(";A 280");		//(= (bool-to-bv (bv-slt EBX  0b00000000000000000000000000000000))   0b1)) ;280
                                            rEIP <= #1 (-EBX); $display(";A 282");		//(= rEIP    (bv-neg EBX ))) ;282
                                        end
                                        else begin
                                            $display(";A 281");		//(= (bool-to-bv (bv-slt EBX  0b00000000000000000000000000000000))   0b0)) ;281
                                            rEIP <= #1 EBX; $display(";A 283");		//(= rEIP    EBX )) ;283
                                        end
                                        RequestPending <= #1 1'b1; $display(";A 284");		//(= RequestPending    0b1)) ;284
                                        ReadRequest <= #1 1'b1; $display(";A 285");		//(= ReadRequest    0b1)) ;285
                                        MemoryFetch <= #1 1'b1; $display(";A 286");		//(= MemoryFetch    0b1)) ;286
                                        CodeFetch <= #1 1'b0; $display(";A 287");		//(= CodeFetch    0b0)) ;287
                                        if ((READY_n == 1'b0)) begin
                                            $display(";A 288");		//(= (bv-comp READY_n  0b0)   0b1)) ;288
                                            RequestPending <= #1 1'b0; $display(";A 290");		//(= RequestPending    0b0)) ;290
                                            uWord = (Datai % 32'b00000000000000001000000000000000); $display(";A 291");		//(= uWord    (bv-smod Datai  0b00000000000000001000000000000000))) ;291
                                            if ((StateBS16 == 1'b1)) begin
                                                $display(";A 292");		//(= (bv-comp StateBS16  0b1)   0b1)) ;292
                                                lWord = (Datai % 32'b00000000000000010000000000000000); $display(";A 294");		//(= lWord    (bv-smod Datai  0b00000000000000010000000000000000))) ;294
                                            end
                                            else begin
                                                $display(";A 293");		//(= (bv-comp StateBS16  0b1)   0b0)) ;293
                                                rEIP <= #1 (rEIP + 32'sb00000000000000000000000000000010); $display(";A 295");		//(= rEIP    (bv-add rEIP  0b00000000000000000000000000000010))) ;295
                                                RequestPending <= #1 1'b1; $display(";A 296");		//(= RequestPending    0b1)) ;296
                                                if ((READY_n == 1'b0)) begin
                                                    $display(";A 297");		//(= (bv-comp READY_n  0b0)   0b1)) ;297
                                                    RequestPending <= #1 1'b0; $display(";A 299");		//(= RequestPending    0b0)) ;299
                                                    lWord = (Datai % 32'b00000000000000010000000000000000); $display(";A 300");		//(= lWord    (bv-smod Datai  0b00000000000000010000000000000000))) ;300
                                                end
                                                else begin
                                                    $display(";A 298");		//(= (bv-comp READY_n  0b0)   0b0)) ;298
                                                end
                                            end
                                            if ((READY_n == 1'b0)) begin
                                                $display(";A 301");		//(= (bv-comp READY_n  0b0)   0b1)) ;301
                                                EAX <= #1 ((uWord * 15'b000000000000000) + lWord); $display(";A 303");		//(= EAX    (bv-add (bv-mul uWord  0b000000000000000) lWord ))) ;303
                                                More = 1'b0; $display(";A 304");		//(= More    0b0)) ;304
                                                Flush = 1'b0; $display(";A 305");		//(= Flush    0b0)) ;305
                                                InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000010); $display(";A 306");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000010))) ;306
                                                InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00010) % 32'b00000000000000000000000000010000); $display(";A 307");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00010) 0b00000000000000000000000000010000))) ;307
                                            end
                                            else begin
                                                $display(";A 302");		//(= (bv-comp READY_n  0b0)   0b0)) ;302
                                            end
                                        end
                                        else begin
                                            $display(";A 289");		//(= (bv-comp READY_n  0b0)   0b0)) ;289
                                        end
                                    end
                                    else begin
                                        $display(";A 279");		//(= (bool-to-bv (bv-ge (bv-sub InstQueueWr_Addr  InstQueueRd_Addr ) 0b00000000000000000000000000000010))   0b0)) ;279
                                        Flush = 1'b0; $display(";A 308");		//(= Flush    0b0)) ;308
                                        More = 1'b1; $display(";A 309");		//(= More    0b1)) ;309
                                    end
                                end
                            8'b10001001 :
                                begin
                                    $display(";A 310");		//(= ( InstQueue InstQueueRd_Addr )   0b10001001)) ;310
                                    if (((InstQueueWr_Addr - InstQueueRd_Addr) >= 32'b00000000000000000000000000000010)) begin
                                        $display(";A 311");		//(= (bool-to-bv (bv-ge (bv-sub InstQueueWr_Addr  InstQueueRd_Addr ) 0b00000000000000000000000000000010))   0b1)) ;311
                                        if ((EBX < 32'sb00000000000000000000000000000000)) begin
                                            $display(";A 313");		//(= (bool-to-bv (bv-slt EBX  0b00000000000000000000000000000000))   0b1)) ;313
                                            rEIP <= #1 EBX; $display(";A 315");		//(= rEIP    EBX )) ;315
                                        end
                                        else begin
                                            $display(";A 314");		//(= (bool-to-bv (bv-slt EBX  0b00000000000000000000000000000000))   0b0)) ;314
                                            rEIP <= #1 EBX; $display(";A 316");		//(= rEIP    EBX )) ;316
                                        end
                                        lWord = (EAX % 32'sb00000000000000010000000000000000); $display(";A 317");		//(= lWord    (bv-smod EAX  0b00000000000000010000000000000000))) ;317
                                        uWord = ((EAX / 32'sb00000000000000010000000000000000) % 32'sb00000000000000001000000000000000); $display(";A 318");		//(= uWord    (bv-smod (bv-sdiv EAX  0b00000000000000010000000000000000) 0b00000000000000001000000000000000))) ;318
                                        RequestPending <= #1 1'b1; $display(";A 319");		//(= RequestPending    0b1)) ;319
                                        ReadRequest <= #1 1'b0; $display(";A 320");		//(= ReadRequest    0b0)) ;320
                                        MemoryFetch <= #1 1'b1; $display(";A 321");		//(= MemoryFetch    0b1)) ;321
                                        CodeFetch <= #1 1'b0; $display(";A 322");		//(= CodeFetch    0b0)) ;322
                                        if (((State == 3'b010) | (State == 3'b100))) begin
                                            $display(";A 323");		//(= (bv-or (bv-comp State  0b010) (bv-comp State  0b100))   0b1)) ;323
                                            Datao <= #1 ((uWord * 15'b000000000000000) + lWord); $display(";A 325");		//(= Datao    (bv-add (bv-mul uWord  0b000000000000000) lWord ))) ;325
                                            if ((READY_n == 1'b0)) begin
                                                $display(";A 326");		//(= (bv-comp READY_n  0b0)   0b1)) ;326
                                                RequestPending <= #1 1'b0; $display(";A 328");		//(= RequestPending    0b0)) ;328
                                                if ((StateBS16 == 1'b0)) begin
                                                    $display(";A 329");		//(= (bv-comp StateBS16  0b0)   0b1)) ;329
                                                    rEIP <= #1 (rEIP + 32'sb00000000000000000000000000000010); $display(";A 331");		//(= rEIP    (bv-add rEIP  0b00000000000000000000000000000010))) ;331
                                                    RequestPending <= #1 1'b1; $display(";A 332");		//(= RequestPending    0b1)) ;332
                                                    ReadRequest <= #1 1'b0; $display(";A 333");		//(= ReadRequest    0b0)) ;333
                                                    MemoryFetch <= #1 1'b1; $display(";A 334");		//(= MemoryFetch    0b1)) ;334
                                                    CodeFetch <= #1 1'b0; $display(";A 335");		//(= CodeFetch    0b0)) ;335
                                                    State2 = 4'b0110; $display(";A 336");		//(= State2    0b0110)) ;336
                                                end
                                                else begin
                                                    $display(";A 330");		//(= (bv-comp StateBS16  0b0)   0b0)) ;330
                                                end
                                                More = 1'b0; $display(";A 337");		//(= More    0b0)) ;337
                                                Flush = 1'b0; $display(";A 338");		//(= Flush    0b0)) ;338
                                                InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000010); $display(";A 339");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000010))) ;339
                                                InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00010) % 32'b00000000000000000000000000010000); $display(";A 340");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00010) 0b00000000000000000000000000010000))) ;340
                                            end
                                            else begin
                                                $display(";A 327");		//(= (bv-comp READY_n  0b0)   0b0)) ;327
                                            end
                                        end
                                        else begin
                                            $display(";A 324");		//(= (bv-or (bv-comp State  0b010) (bv-comp State  0b100))   0b0)) ;324
                                        end
                                    end
                                    else begin
                                        $display(";A 312");		//(= (bool-to-bv (bv-ge (bv-sub InstQueueWr_Addr  InstQueueRd_Addr ) 0b00000000000000000000000000000010))   0b0)) ;312
                                        Flush = 1'b0; $display(";A 341");		//(= Flush    0b0)) ;341
                                        More = 1'b1; $display(";A 342");		//(= More    0b1)) ;342
                                    end
                                end
                            8'b11100100 :
                                begin
                                    $display(";A 343");		//(= ( InstQueue InstQueueRd_Addr )   0b11100100)) ;343
                                    if (((InstQueueWr_Addr - InstQueueRd_Addr) >= 32'b00000000000000000000000000000010)) begin
                                        $display(";A 344");		//(= (bool-to-bv (bv-ge (bv-sub InstQueueWr_Addr  InstQueueRd_Addr ) 0b00000000000000000000000000000010))   0b1)) ;344
                                        rEIP <= #1 (InstQueueRd_Addr + 5'b00001); $display(";A 346");		//(= rEIP    (bv-add InstQueueRd_Addr  0b00001))) ;346
                                        RequestPending <= #1 1'b1; $display(";A 347");		//(= RequestPending    0b1)) ;347
                                        ReadRequest <= #1 1'b1; $display(";A 348");		//(= ReadRequest    0b1)) ;348
                                        MemoryFetch <= #1 1'b0; $display(";A 349");		//(= MemoryFetch    0b0)) ;349
                                        CodeFetch <= #1 1'b0; $display(";A 350");		//(= CodeFetch    0b0)) ;350
                                        if ((READY_n == 1'b0)) begin
                                            $display(";A 351");		//(= (bv-comp READY_n  0b0)   0b1)) ;351
                                            RequestPending <= #1 1'b0; $display(";A 353");		//(= RequestPending    0b0)) ;353
                                            EAX <= #1 Datai; $display(";A 354");		//(= EAX    Datai )) ;354
                                            InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000010); $display(";A 355");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000010))) ;355
                                            InstQueueRd_Addr = (InstQueueRd_Addr + 5'b00010); $display(";A 356");		//(= InstQueueRd_Addr    (bv-add InstQueueRd_Addr  0b00010))) ;356
                                            Flush = 1'b0; $display(";A 357");		//(= Flush    0b0)) ;357
                                            More = 1'b0; $display(";A 358");		//(= More    0b0)) ;358
                                        end
                                        else begin
                                            $display(";A 352");		//(= (bv-comp READY_n  0b0)   0b0)) ;352
                                        end
                                    end
                                    else begin
                                        $display(";A 345");		//(= (bool-to-bv (bv-ge (bv-sub InstQueueWr_Addr  InstQueueRd_Addr ) 0b00000000000000000000000000000010))   0b0)) ;345
                                        Flush = 1'b0; $display(";A 359");		//(= Flush    0b0)) ;359
                                        More = 1'b1; $display(";A 360");		//(= More    0b1)) ;360
                                    end
                                end
                            8'b11100110 :
                                begin
                                    $display(";A 361");		//(= ( InstQueue InstQueueRd_Addr )   0b11100110)) ;361
                                    if (((InstQueueWr_Addr - InstQueueRd_Addr) >= 32'b00000000000000000000000000000010)) begin
                                        $display(";A 362");		//(= (bool-to-bv (bv-ge (bv-sub InstQueueWr_Addr  InstQueueRd_Addr ) 0b00000000000000000000000000000010))   0b1)) ;362
                                        rEIP <= #1 (InstQueueRd_Addr + 5'b00001); $display(";A 364");		//(= rEIP    (bv-add InstQueueRd_Addr  0b00001))) ;364
                                        RequestPending <= #1 1'b1; $display(";A 365");		//(= RequestPending    0b1)) ;365
                                        ReadRequest <= #1 1'b0; $display(";A 366");		//(= ReadRequest    0b0)) ;366
                                        MemoryFetch <= #1 1'b0; $display(";A 367");		//(= MemoryFetch    0b0)) ;367
                                        CodeFetch <= #1 1'b0; $display(";A 368");		//(= CodeFetch    0b0)) ;368
                                        if (((State == 3'b010) | (State == 3'b100))) begin
                                            $display(";A 369");		//(= (bv-or (bv-comp State  0b010) (bv-comp State  0b100))   0b1)) ;369
                                            fWord = (EAX % 32'sb00000000000000010000000000000000); $display(";A 371");		//(= fWord    (bv-smod EAX  0b00000000000000010000000000000000))) ;371
                                            Datao <= #1 fWord; $display(";A 372");		//(= Datao    fWord )) ;372
                                            if ((READY_n == 1'b0)) begin
                                                $display(";A 373");		//(= (bv-comp READY_n  0b0)   0b1)) ;373
                                                RequestPending <= #1 1'b0; $display(";A 375");		//(= RequestPending    0b0)) ;375
                                                InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000010); $display(";A 376");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000010))) ;376
                                                InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00010) % 32'b00000000000000000000000000010000); $display(";A 377");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00010) 0b00000000000000000000000000010000))) ;377
                                                Flush = 1'b0; $display(";A 378");		//(= Flush    0b0)) ;378
                                                More = 1'b0; $display(";A 379");		//(= More    0b0)) ;379
                                            end
                                            else begin
                                                $display(";A 374");		//(= (bv-comp READY_n  0b0)   0b0)) ;374
                                            end
                                        end
                                        else begin
                                            $display(";A 370");		//(= (bv-or (bv-comp State  0b010) (bv-comp State  0b100))   0b0)) ;370
                                        end
                                    end
                                    else begin
                                        $display(";A 363");		//(= (bool-to-bv (bv-ge (bv-sub InstQueueWr_Addr  InstQueueRd_Addr ) 0b00000000000000000000000000000010))   0b0)) ;363
                                        Flush = 1'b0; $display(";A 380");		//(= Flush    0b0)) ;380
                                        More = 1'b1; $display(";A 381");		//(= More    0b1)) ;381
                                    end
                                end
                            8'b00000100 :
                                begin
                                    $display(";A 382");		//(= ( InstQueue InstQueueRd_Addr )   0b00000100)) ;382
                                    InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000001); $display(";A 383");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000001))) ;383
                                    InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 384");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00001) 0b00000000000000000000000000010000))) ;384
                                    Flush = 1'b0; $display(";A 385");		//(= Flush    0b0)) ;385
                                    More = 1'b0; $display(";A 386");		//(= More    0b0)) ;386
                                end
                            8'b00000101 :
                                begin
                                    $display(";A 387");		//(= ( InstQueue InstQueueRd_Addr )   0b00000101)) ;387
                                    InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000001); $display(";A 388");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000001))) ;388
                                    InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 389");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00001) 0b00000000000000000000000000010000))) ;389
                                    Flush = 1'b0; $display(";A 390");		//(= Flush    0b0)) ;390
                                    More = 1'b0; $display(";A 391");		//(= More    0b0)) ;391
                                end
                            8'b11010000 :
                                begin
                                    $display(";A 392");		//(= ( InstQueue InstQueueRd_Addr )   0b11010000)) ;392
                                    InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000010); $display(";A 393");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000010))) ;393
                                    InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00010) % 32'b00000000000000000000000000010000); $display(";A 394");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00010) 0b00000000000000000000000000010000))) ;394
                                    Flush = 1'b0; $display(";A 395");		//(= Flush    0b0)) ;395
                                    More = 1'b0; $display(";A 396");		//(= More    0b0)) ;396
                                end
                            8'b11000000 :
                                begin
                                    $display(";A 397");		//(= ( InstQueue InstQueueRd_Addr )   0b11000000)) ;397
                                    InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000010); $display(";A 398");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000010))) ;398
                                    InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00010) % 32'b00000000000000000000000000010000); $display(";A 399");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00010) 0b00000000000000000000000000010000))) ;399
                                    Flush = 1'b0; $display(";A 400");		//(= Flush    0b0)) ;400
                                    More = 1'b0; $display(";A 401");		//(= More    0b0)) ;401
                                end
                            8'b01000000 :
                                begin
                                    $display(";A 402");		//(= ( InstQueue InstQueueRd_Addr )   0b01000000)) ;402
                                    EAX <= #1 (EAX + 32'sb00000000000000000000000000000001); $display(";A 403");		//(= EAX    (bv-add EAX  0b00000000000000000000000000000001))) ;403
                                    InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000001); $display(";A 404");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000001))) ;404
                                    InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 405");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00001) 0b00000000000000000000000000010000))) ;405
                                    Flush = 1'b0; $display(";A 406");		//(= Flush    0b0)) ;406
                                    More = 1'b0; $display(";A 407");		//(= More    0b0)) ;407
                                end
                            8'b01000011 :
                                begin
                                    $display(";A 408");		//(= ( InstQueue InstQueueRd_Addr )   0b01000011)) ;408
                                    EBX <= #1 (EBX + 32'sb00000000000000000000000000000001); $display(";A 409");		//(= EBX    (bv-add EBX  0b00000000000000000000000000000001))) ;409
                                    InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000001); $display(";A 410");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000001))) ;410
                                    InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 411");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00001) 0b00000000000000000000000000010000))) ;411
                                    Flush = 1'b0; $display(";A 412");		//(= Flush    0b0)) ;412
                                    More = 1'b0; $display(";A 413");		//(= More    0b0)) ;413
                                end
                            default:
                                begin
                                    $display(";A 414");		//(= (and (/= ( InstQueue InstQueueRd_Addr ) 0b10010000) (/= ( InstQueue InstQueueRd_Addr ) 0b01100110) (/= ( InstQueue InstQueueRd_Addr ) 0b11101011) (/= ( InstQueue InstQueueRd_Addr ) 0b11101001) (/= ( InstQueue InstQueueRd_Addr ) 0b11101010) (/= ( InstQueue InstQueueRd_Addr ) 0b10110000) (/= ( InstQueue InstQueueRd_Addr ) 0b10111000) (/= ( InstQueue InstQueueRd_Addr ) 0b10111011) (/= ( InstQueue InstQueueRd_Addr ) 0b10001011) (/= ( InstQueue InstQueueRd_Addr ) 0b10001001) (/= ( InstQueue InstQueueRd_Addr ) 0b11100100) (/= ( InstQueue InstQueueRd_Addr ) 0b11100110) (/= ( InstQueue InstQueueRd_Addr ) 0b00000100) (/= ( InstQueue InstQueueRd_Addr ) 0b00000101) (/= ( InstQueue InstQueueRd_Addr ) 0b11010000) (/= ( InstQueue InstQueueRd_Addr ) 0b11000000) (/= ( InstQueue InstQueueRd_Addr ) 0b01000000) (/= ( InstQueue InstQueueRd_Addr ) 0b01000011))   true)) ;414
                                    InstAddrPointer = (InstAddrPointer + 32'sb00000000000000000000000000000001); $display(";A 415");		//(= InstAddrPointer    (bv-add InstAddrPointer  0b00000000000000000000000000000001))) ;415
                                    InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 416");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00001) 0b00000000000000000000000000010000))) ;416
                                    Flush = 1'b0; $display(";A 417");		//(= Flush    0b0)) ;417
                                    More = 1'b0; $display(";A 418");		//(= More    0b0)) ;418
                                end
                        endcase
                        if (((~(InstQueueRd_Addr < InstQueueWr_Addr)) | ((((32'b00000000000000000000000000001111 - InstQueueRd_Addr) < 32'b00000000000000000000000000000100) | Flush) | More))) begin
                            $display(";A 419");		//(= (bv-or (bv-not (bool-to-bv (bv-lt InstQueueRd_Addr  InstQueueWr_Addr ))) (bv-or (bv-or (bool-to-bv (bv-lt (bv-sub 0b00000000000000000000000000001111 InstQueueRd_Addr ) 0b00000000000000000000000000000100)) Flush ) More ))   0b1)) ;419
                            State2 = 4'b0111; $display(";A 421");		//(= State2    0b0111)) ;421
                        end
                        else begin
                            $display(";A 420");		//(= (bv-or (bv-not (bool-to-bv (bv-lt InstQueueRd_Addr  InstQueueWr_Addr ))) (bv-or (bv-or (bool-to-bv (bv-lt (bv-sub 0b00000000000000000000000000001111 InstQueueRd_Addr ) 0b00000000000000000000000000000100)) Flush ) More ))   0b0)) ;420
                        end
                    end
                4'b0110 :
                    begin
                        $display(";A 422");		//(= State2    0b0110)) ;422
                        Datao <= #1 ((uWord * 15'b000000000000000) + lWord); $display(";A 423");		//(= Datao    (bv-add (bv-mul uWord  0b000000000000000) lWord ))) ;423
                        if ((READY_n == 1'b0)) begin
                            $display(";A 424");		//(= (bv-comp READY_n  0b0)   0b1)) ;424
                            RequestPending <= #1 1'b0; $display(";A 426");		//(= RequestPending    0b0)) ;426
                            State2 = 4'b0101; $display(";A 427");		//(= State2    0b0101)) ;427
                        end
                        else begin
                            $display(";A 425");		//(= (bv-comp READY_n  0b0)   0b0)) ;425
                        end
                    end
                4'b0111 :
                    begin
                        $display(";A 428");		//(= State2    0b0111)) ;428
                        if (Flush) begin
                            $display(";A 429");		//(= Flush    0b1)) ;429
                            InstQueueRd_Addr = 5'b00001; $display(";A 431");		//(= InstQueueRd_Addr    0b00001)) ;431
                            InstQueueWr_Addr = 5'b00001; $display(";A 432");		//(= InstQueueWr_Addr    0b00001)) ;432
                            if ((InstAddrPointer < 32'sb00000000000000000000000000000000)) begin
                                $display(";A 433");		//(= (bool-to-bv (bv-slt InstAddrPointer  0b00000000000000000000000000000000))   0b1)) ;433
                                fWord = (-InstAddrPointer); $display(";A 435");		//(= fWord    (bv-neg InstAddrPointer ))) ;435
                            end
                            else begin
                                $display(";A 434");		//(= (bool-to-bv (bv-slt InstAddrPointer  0b00000000000000000000000000000000))   0b0)) ;434
                                fWord = InstAddrPointer; $display(";A 436");		//(= fWord    InstAddrPointer )) ;436
                            end
                            if (((fWord % 32'sb00000000000000000000000000000010) == 32'sb00000000000000000000000000000001)) begin
                                $display(";A 437");		//(= (bv-comp (bv-smod fWord  0b00000000000000000000000000000010) 0b00000000000000000000000000000001)   0b1)) ;437
                                InstQueueRd_Addr = ((InstQueueRd_Addr + (fWord % 32'b00000000000000000000000000000100)) % 32'b00000000000000000000000000010000); $display(";A 439");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  (bv-smod fWord  0b00000000000000000000000000000100)) 0b00000000000000000000000000010000))) ;439
                            end
                            else begin
                                $display(";A 438");		//(= (bv-comp (bv-smod fWord  0b00000000000000000000000000000010) 0b00000000000000000000000000000001)   0b0)) ;438
                            end
                        end
                        else begin
                            $display(";A 430");		//(= Flush    0b0)) ;430
                        end
                        if (((32'b00000000000000000000000000001111 - InstQueueRd_Addr) < 32'b00000000000000000000000000000011)) begin
                            $display(";A 440");		//(= (bool-to-bv (bv-lt (bv-sub 0b00000000000000000000000000001111 InstQueueRd_Addr ) 0b00000000000000000000000000000011))   0b1)) ;440
                            State2 = 4'b1000; $display(";A 442");		//(= State2    0b1000)) ;442
                            InstQueueWr_Addr = 5'b00000; $display(";A 443");		//(= InstQueueWr_Addr    0b00000)) ;443
                        end
                        else begin
                            $display(";A 441");		//(= (bool-to-bv (bv-lt (bv-sub 0b00000000000000000000000000001111 InstQueueRd_Addr ) 0b00000000000000000000000000000011))   0b0)) ;441
                            State2 = 4'b1001; $display(";A 444");		//(= State2    0b1001)) ;444
                        end
                    end
                4'b1000 :
                    begin
                        $display(";A 445");		//(= State2    0b1000)) ;445
                        if ((InstQueueRd_Addr <= 5'b01111)) begin
                            $display(";A 446");		//(= (bool-to-bv (bv-le InstQueueRd_Addr  0b01111))   0b1)) ;446
                            InstQueue[InstQueueWr_Addr] = InstQueue[InstQueueRd_Addr]; $display(";A 448");		//(= ( InstQueue InstQueueWr_Addr )   ( InstQueue InstQueueRd_Addr ))) ;448
                            InstQueueRd_Addr = ((InstQueueRd_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 449");		//(= InstQueueRd_Addr    (bv-smod (bv-add InstQueueRd_Addr  0b00001) 0b00000000000000000000000000010000))) ;449
                            InstQueueWr_Addr = ((InstQueueWr_Addr + 5'b00001) % 32'b00000000000000000000000000010000); $display(";A 450");		//(= InstQueueWr_Addr    (bv-smod (bv-add InstQueueWr_Addr  0b00001) 0b00000000000000000000000000010000))) ;450
                            State2 = 4'b1000; $display(";A 451");		//(= State2    0b1000)) ;451
                        end
                        else begin
                            $display(";A 447");		//(= (bool-to-bv (bv-le InstQueueRd_Addr  0b01111))   0b0)) ;447
                            InstQueueRd_Addr = 5'b00000; $display(";A 452");		//(= InstQueueRd_Addr    0b00000)) ;452
                            State2 = 4'b1001; $display(";A 453");		//(= State2    0b1001)) ;453
                        end
                    end
                4'b1001 :
                    begin
                        $display(";A 454");		//(= State2    0b1001)) ;454
                        rEIP <= #1 PhyAddrPointer; $display(";A 455");		//(= rEIP    PhyAddrPointer )) ;455
                        State2 = 4'b0001; $display(";A 456");		//(= State2    0b0001)) ;456
                    end
            endcase
        end
    if (sig_display) begin
        $display(";F ADS_n = %b;Address = %b;BE_n = %b;ByteEnable = %b;CodeFetch = %b;D_C_n = %b;DataWidth = %b;Datao = %b;EAX = %b;EBX = %b;Extended = %b;Flush = %b;InstAddrPointer = %b;InstQueue = %b;InstQueueRd_Addr = %b;InstQueueWr_Addr = %b;M_IO_n = %b;MemoryFetch = %b;More = %b;NonAligned = %b;PhyAddrPointer = %b;ReadRequest = %b;RequestPending = %b;State = %b;State2 = %b;StateBS16 = %b;StateNA = %b;W_R_n = %b;fWord = %b;lWord = %b;rEIP = %b;uWord = %b;",ADS_n,Address,BE_n,ByteEnable,CodeFetch,D_C_n,DataWidth,Datao,EAX,EBX,Extended,Flush,InstAddrPointer,InstQueue,InstQueueRd_Addr,InstQueueWr_Addr,M_IO_n,MemoryFetch,More,NonAligned,PhyAddrPointer,ReadRequest,RequestPending,State,State2,StateBS16,StateNA,W_R_n,fWord,lWord,rEIP,uWord);
        end
    end

    // Following code segment is generated from /home/ziyue/researchlib/Micro_Eletronic/STSearch/tests/b15/src/b15.v:716
    always @(posedge RESET or posedge CLOCK) begin
        if ((RESET == 1'b1)) begin
            ByteEnable <= #1 4'b0000; $display(";A 459");		//(= ByteEnable    0b0000)) ;459
            NonAligned <= #1 1'b0; $display(";A 460");		//(= NonAligned    0b0)) ;460
        end
        else begin
            case (DataWidth)
                32'sb00000000000000000000000000000000 :
                    begin
                        $display(";A 461");		//(= DataWidth    0b00000000000000000000000000000000)) ;461
                        case ((rEIP % 32'sb00000000000000000000000000000100))
                            32'sb00000000000000000000000000000000 :
                                begin
                                    $display(";A 462");		//(= (bv-smod rEIP  0b00000000000000000000000000000100)   0b00000000000000000000000000000000)) ;462
                                    ByteEnable <= #1 4'b1110; $display(";A 463");		//(= ByteEnable    0b1110)) ;463
                                end
                            32'sb00000000000000000000000000000001 :
                                begin
                                    $display(";A 464");		//(= (bv-smod rEIP  0b00000000000000000000000000000100)   0b00000000000000000000000000000001)) ;464
                                    ByteEnable <= #1 4'b1101; $display(";A 465");		//(= ByteEnable    0b1101)) ;465
                                end
                            32'sb00000000000000000000000000000010 :
                                begin
                                    $display(";A 466");		//(= (bv-smod rEIP  0b00000000000000000000000000000100)   0b00000000000000000000000000000010)) ;466
                                    ByteEnable <= #1 4'b1011; $display(";A 467");		//(= ByteEnable    0b1011)) ;467
                                end
                            32'sb00000000000000000000000000000011 :
                                begin
                                    $display(";A 468");		//(= (bv-smod rEIP  0b00000000000000000000000000000100)   0b00000000000000000000000000000011)) ;468
                                    ByteEnable <= #1 4'b0111; $display(";A 469");		//(= ByteEnable    0b0111)) ;469
                                end
                            default:
                                begin
                                    $display(";A 470");		//(= (and (/= (bv-smod rEIP  0b00000000000000000000000000000100) 0b00000000000000000000000000000000) (/= (bv-smod rEIP  0b00000000000000000000000000000100) 0b00000000000000000000000000000001) (/= (bv-smod rEIP  0b00000000000000000000000000000100) 0b00000000000000000000000000000010) (/= (bv-smod rEIP  0b00000000000000000000000000000100) 0b00000000000000000000000000000011))   true)) ;470
                                    begin
                                    end
                                end
                        endcase
                    end
                32'sb00000000000000000000000000000001 :
                    begin
                        $display(";A 471");		//(= DataWidth    0b00000000000000000000000000000001)) ;471
                        case ((rEIP % 32'sb00000000000000000000000000000100))
                            32'sb00000000000000000000000000000000 :
                                begin
                                    $display(";A 472");		//(= (bv-smod rEIP  0b00000000000000000000000000000100)   0b00000000000000000000000000000000)) ;472
                                    ByteEnable <= #1 4'b1100; $display(";A 473");		//(= ByteEnable    0b1100)) ;473
                                    NonAligned <= #1 1'b0; $display(";A 474");		//(= NonAligned    0b0)) ;474
                                end
                            32'sb00000000000000000000000000000001 :
                                begin
                                    $display(";A 475");		//(= (bv-smod rEIP  0b00000000000000000000000000000100)   0b00000000000000000000000000000001)) ;475
                                    ByteEnable <= #1 4'b1001; $display(";A 476");		//(= ByteEnable    0b1001)) ;476
                                    NonAligned <= #1 1'b0; $display(";A 477");		//(= NonAligned    0b0)) ;477
                                end
                            32'sb00000000000000000000000000000010 :
                                begin
                                    $display(";A 478");		//(= (bv-smod rEIP  0b00000000000000000000000000000100)   0b00000000000000000000000000000010)) ;478
                                    ByteEnable <= #1 4'b0011; $display(";A 479");		//(= ByteEnable    0b0011)) ;479
                                    NonAligned <= #1 1'b0; $display(";A 480");		//(= NonAligned    0b0)) ;480
                                end
                            32'sb00000000000000000000000000000011 :
                                begin
                                    $display(";A 481");		//(= (bv-smod rEIP  0b00000000000000000000000000000100)   0b00000000000000000000000000000011)) ;481
                                    ByteEnable <= #1 4'b0111; $display(";A 482");		//(= ByteEnable    0b0111)) ;482
                                    NonAligned <= #1 1'b1; $display(";A 483");		//(= NonAligned    0b1)) ;483
                                end
                            default:
                                begin
                                    $display(";A 484");		//(= (and (/= (bv-smod rEIP  0b00000000000000000000000000000100) 0b00000000000000000000000000000000) (/= (bv-smod rEIP  0b00000000000000000000000000000100) 0b00000000000000000000000000000001) (/= (bv-smod rEIP  0b00000000000000000000000000000100) 0b00000000000000000000000000000010) (/= (bv-smod rEIP  0b00000000000000000000000000000100) 0b00000000000000000000000000000011))   true)) ;484
                                    begin
                                    end
                                end
                        endcase
                    end
                32'sb00000000000000000000000000000010 :
                    begin
                        $display(";A 485");		//(= DataWidth    0b00000000000000000000000000000010)) ;485
                        case ((rEIP % 32'sb00000000000000000000000000000100))
                            32'sb00000000000000000000000000000000 :
                                begin
                                    $display(";A 486");		//(= (bv-smod rEIP  0b00000000000000000000000000000100)   0b00000000000000000000000000000000)) ;486
                                    ByteEnable <= #1 4'b0000; $display(";A 487");		//(= ByteEnable    0b0000)) ;487
                                    NonAligned <= #1 1'b0; $display(";A 488");		//(= NonAligned    0b0)) ;488
                                end
                            32'sb00000000000000000000000000000001 :
                                begin
                                    $display(";A 489");		//(= (bv-smod rEIP  0b00000000000000000000000000000100)   0b00000000000000000000000000000001)) ;489
                                    ByteEnable <= #1 4'b0001; $display(";A 490");		//(= ByteEnable    0b0001)) ;490
                                    NonAligned <= #1 1'b1; $display(";A 491");		//(= NonAligned    0b1)) ;491
                                end
                            32'sb00000000000000000000000000000010 :
                                begin
                                    $display(";A 492");		//(= (bv-smod rEIP  0b00000000000000000000000000000100)   0b00000000000000000000000000000010)) ;492
                                    NonAligned <= #1 1'b1; $display(";A 493");		//(= NonAligned    0b1)) ;493
                                    ByteEnable <= #1 4'b0011; $display(";A 494");		//(= ByteEnable    0b0011)) ;494
                                end
                            32'sb00000000000000000000000000000011 :
                                begin
                                    $display(";A 495");		//(= (bv-smod rEIP  0b00000000000000000000000000000100)   0b00000000000000000000000000000011)) ;495
                                    NonAligned <= #1 1'b1; $display(";A 496");		//(= NonAligned    0b1)) ;496
                                    ByteEnable <= #1 4'b0111; $display(";A 497");		//(= ByteEnable    0b0111)) ;497
                                end
                            default:
                                begin
                                    $display(";A 498");		//(= (and (/= (bv-smod rEIP  0b00000000000000000000000000000100) 0b00000000000000000000000000000000) (/= (bv-smod rEIP  0b00000000000000000000000000000100) 0b00000000000000000000000000000001) (/= (bv-smod rEIP  0b00000000000000000000000000000100) 0b00000000000000000000000000000010) (/= (bv-smod rEIP  0b00000000000000000000000000000100) 0b00000000000000000000000000000011))   true)) ;498
                                    begin
                                    end
                                end
                        endcase
                    end
                default:
                    begin
                        $display(";A 499");		//(= (and (/= DataWidth  0b00000000000000000000000000000000) (/= DataWidth  0b00000000000000000000000000000001) (/= DataWidth  0b00000000000000000000000000000010))   true)) ;499
                        begin
                        end
                    end
            endcase
        end
    if (sig_display) begin
        $display(";F ADS_n = %b;Address = %b;BE_n = %b;ByteEnable = %b;CodeFetch = %b;D_C_n = %b;DataWidth = %b;Datao = %b;EAX = %b;EBX = %b;Extended = %b;Flush = %b;InstAddrPointer = %b;InstQueue = %b;InstQueueRd_Addr = %b;InstQueueWr_Addr = %b;M_IO_n = %b;MemoryFetch = %b;More = %b;NonAligned = %b;PhyAddrPointer = %b;ReadRequest = %b;RequestPending = %b;State = %b;State2 = %b;StateBS16 = %b;StateNA = %b;W_R_n = %b;fWord = %b;lWord = %b;rEIP = %b;uWord = %b;",ADS_n,Address,BE_n,ByteEnable,CodeFetch,D_C_n,DataWidth,Datao,EAX,EBX,Extended,Flush,InstAddrPointer,InstQueue,InstQueueRd_Addr,InstQueueWr_Addr,M_IO_n,MemoryFetch,More,NonAligned,PhyAddrPointer,ReadRequest,RequestPending,State,State2,StateBS16,StateNA,W_R_n,fWord,lWord,rEIP,uWord);
        end
    end

endmodule

