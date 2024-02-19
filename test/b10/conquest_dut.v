// Following code segment is generated from /home/meng/Code/concolic-testing/test/b10/src/b10.v:2
module b10(r_button, g_button, key, start, reset, test, cts, ctr, rts, rtr, clock, v_in, v_out, __obs);
    input r_button;
    input g_button;
    input key;
    input start;
    input reset;
    input test;
    output cts;
    output ctr;
    input rts;
    input rtr;
    input clock;
    input [3:0] v_in;
    output [3:0] v_out;
    input __obs;

    reg ctr = 1'b0;
    reg cts = 1'b0;
    reg last_g = 1'b0;
    reg last_r = 1'b0;
    reg [3:0] sign = 4'b0;
    reg [3:0] stato = 4'b0;
    reg [3:0] v_out = 4'b0;
    reg voto0 = 1'b0;
    reg voto1 = 1'b0;
    reg voto2 = 1'b0;
    reg voto3 = 1'b0;

    // Following code segment is generated from /home/meng/Code/concolic-testing/test/b10/src/b10.v:38
    always @(posedge clock) begin
        if ((reset == 1'b1)) begin
            stato <= #1 4'h0; $display(";A 2");		//(assert (= stato    0h0)) ;2
            voto0 <= #1 1'b0; $display(";A 3");		//(assert (= voto0    0b0)) ;3
            voto1 <= #1 1'b0; $display(";A 4");		//(assert (= voto1    0b0)) ;4
            voto2 <= #1 1'b0; $display(";A 5");		//(assert (= voto2    0b0)) ;5
            voto3 <= #1 1'b0; $display(";A 6");		//(assert (= voto3    0b0)) ;6
            sign <= #1 4'h0; $display(";A 7");		//(assert (= sign    0h0)) ;7
            last_g <= #1 1'b0; $display(";A 8");		//(assert (= last_g    0b0)) ;8
            last_r <= #1 1'b0; $display(";A 9");		//(assert (= last_r    0b0)) ;9
            cts <= #1 1'b0; $display(";A 10");		//(assert (= cts    0b0)) ;10
            ctr <= #1 1'b0; $display(";A 11");		//(assert (= ctr    0b0)) ;11
            v_out <= #1 4'h0; $display(";A 12");		//(assert (= v_out    0h0)) ;12
        end
        else begin
            case (stato)
                4'h0 :
                    begin
                        $display(";A 13");		//(assert (= stato    0h0)) ;13
                        voto0 <= #1 1'b0; $display(";A 14");		//(assert (= voto0    0b0)) ;14
                        voto1 <= #1 1'b0; $display(";A 15");		//(assert (= voto1    0b0)) ;15
                        voto2 <= #1 1'b0; $display(";A 16");		//(assert (= voto2    0b0)) ;16
                        voto3 <= #1 1'b0; $display(";A 17");		//(assert (= voto3    0b0)) ;17
                        cts <= #1 1'b0; $display(";A 18");		//(assert (= cts    0b0)) ;18
                        ctr <= #1 1'b0; $display(";A 19");		//(assert (= ctr    0b0)) ;19
                        if ((test == 1'b0)) begin
                            $display(";A 20");		//(assert (= (bv-comp test  0b0)   0b1)) ;20
                            sign <= #1 4'h0; $display(";A 22");		//(assert (= sign    0h0)) ;22
                            stato <= #1 4'h9; $display(";A 23");		//(assert (= stato    0h9)) ;23
                        end
                        else begin
                            $display(";A 21");		//(assert (= (bv-comp test  0b0)   0b0)) ;21
                            voto0 <= #1 1'b0; $display(";A 24");		//(assert (= voto0    0b0)) ;24
                            voto1 <= #1 1'b0; $display(";A 25");		//(assert (= voto1    0b0)) ;25
                            voto2 <= #1 1'b0; $display(";A 26");		//(assert (= voto2    0b0)) ;26
                            voto3 <= #1 1'b0; $display(";A 27");		//(assert (= voto3    0b0)) ;27
                            stato <= #1 4'h1; $display(";A 28");		//(assert (= stato    0h1)) ;28
                        end
                    end
                4'h1 :
                    begin
                        $display(";A 29");		//(assert (= stato    0h1)) ;29
                        if ((start == 1'b1)) begin
                            $display(";A 30");		//(assert (= (bv-comp start  0b1)   0b1)) ;30
                            voto0 <= #1 1'b0; $display(";A 32");		//(assert (= voto0    0b0)) ;32
                            voto1 <= #1 1'b0; $display(";A 33");		//(assert (= voto1    0b0)) ;33
                            voto2 <= #1 1'b0; $display(";A 34");		//(assert (= voto2    0b0)) ;34
                            voto3 <= #1 1'b0; $display(";A 35");		//(assert (= voto3    0b0)) ;35
                            stato <= #1 4'h2; $display(";A 36");		//(assert (= stato    0h2)) ;36
                        end
                        else begin
                            $display(";A 31");		//(assert (= (bv-comp start  0b1)   0b0)) ;31
                        end
                        if ((rtr == 1'b1)) begin
                            $display(";A 37");		//(assert (= (bv-comp rtr  0b1)   0b1)) ;37
                            cts <= #1 1'b1; $display(";A 39");		//(assert (= cts    0b1)) ;39
                        end
                        else begin
                            $display(";A 38");		//(assert (= (bv-comp rtr  0b1)   0b0)) ;38
                        end
                        if ((rtr == 1'b0)) begin
                            $display(";A 40");		//(assert (= (bv-comp rtr  0b0)   0b1)) ;40
                            cts <= #1 1'b0; $display(";A 42");		//(assert (= cts    0b0)) ;42
                        end
                        else begin
                            $display(";A 41");		//(assert (= (bv-comp rtr  0b0)   0b0)) ;41
                        end
                    end
                4'h2 :
                    begin
                        $display(";A 43");		//(assert (= stato    0h2)) ;43
                        if ((start == 1'b0)) begin
                            $display(";A 44");		//(assert (= (bv-comp start  0b0)   0b1)) ;44
                            stato <= #1 4'h3; $display(";A 46");		//(assert (= stato    0h3)) ;46
                        end
                        else begin
                            $display(";A 45");		//(assert (= (bv-comp start  0b0)   0b0)) ;45
                            if ((key == 1'b1)) begin
                                $display(";A 47");		//(assert (= (bv-comp key  0b1)   0b1)) ;47
                                voto0 <= #1 key; $display(";A 49");		//(assert (= voto0    key )) ;49
                                if ((((g_button ^ last_g) & g_button) == 1'b1)) begin
                                    $display(";A 50");		//(assert (= (bv-comp (bv-and (bv-xor g_button  last_g ) g_button ) 0b1)   0b1)) ;50
                                    voto1 <= #1 (~voto1); $display(";A 52");		//(assert (= voto1    (bv-not voto1 ))) ;52
                                end
                                else begin
                                    $display(";A 51");		//(assert (= (bv-comp (bv-and (bv-xor g_button  last_g ) g_button ) 0b1)   0b0)) ;51
                                end
                                if ((((r_button ^ last_r) & r_button) == 1'b1)) begin
                                    $display(";A 53");		//(assert (= (bv-comp (bv-and (bv-xor r_button  last_r ) r_button ) 0b1)   0b1)) ;53
                                    voto2 <= #1 (~voto2); $display(";A 55");		//(assert (= voto2    (bv-not voto2 ))) ;55
                                end
                                else begin
                                    $display(";A 54");		//(assert (= (bv-comp (bv-and (bv-xor r_button  last_r ) r_button ) 0b1)   0b0)) ;54
                                end
                                last_g <= #1 g_button; $display(";A 56");		//(assert (= last_g    g_button )) ;56
                                last_r <= #1 r_button; $display(";A 57");		//(assert (= last_r    r_button )) ;57
                            end
                            else begin
                                $display(";A 48");		//(assert (= (bv-comp key  0b1)   0b0)) ;48
                                voto0 <= #1 1'b0; $display(";A 58");		//(assert (= voto0    0b0)) ;58
                                voto1 <= #1 1'b0; $display(";A 59");		//(assert (= voto1    0b0)) ;59
                                voto2 <= #1 1'b0; $display(";A 60");		//(assert (= voto2    0b0)) ;60
                                voto3 <= #1 1'b0; $display(";A 61");		//(assert (= voto3    0b0)) ;61
                            end
                        end
                    end
                4'h3 :
                    begin
                        $display(";A 62");		//(assert (= stato    0h3)) ;62
                        voto3 <= #1 (voto0 ^ (voto1 ^ voto2)); $display(";A 63");		//(assert (= voto3    (bv-xor voto0  (bv-xor voto1  voto2 )))) ;63
                        stato <= #1 4'h4; $display(";A 64");		//(assert (= stato    0h4)) ;64
                        voto0 <= #1 1'b0; $display(";A 65");		//(assert (= voto0    0b0)) ;65
                    end
                4'h4 :
                    begin
                        $display(";A 66");		//(assert (= stato    0h4)) ;66
                        if ((rtr == 1'b1)) begin
                            $display(";A 67");		//(assert (= (bv-comp rtr  0b1)   0b1)) ;67
                            v_out <= #1 {voto3, voto2, voto1, voto0}; $display(";A 69");		//(assert (= v_out    (bv-concat voto3  voto2  voto1  voto0 ))) ;69
                            cts <= #1 1'b1; $display(";A 70");		//(assert (= cts    0b1)) ;70
                            if (((((voto0 == 1'b0) && (voto1 == 1'b1)) && (voto2 == 1'b1)) && (voto3 == 1'b0))) begin
                                $display(";A 71");		//(assert (= (bv-and (bv-and (bv-and (bv-comp voto0  0b0) (bv-comp voto1  0b1)) (bv-comp voto2  0b1)) (bv-comp voto3  0b0))   0b1)) ;71
                                stato <= #1 4'h8; $display(";A 73");		//(assert (= stato    0h8)) ;73
                            end
                            else begin
                                $display(";A 72");		//(assert (= (bv-and (bv-and (bv-and (bv-comp voto0  0b0) (bv-comp voto1  0b1)) (bv-comp voto2  0b1)) (bv-comp voto3  0b0))   0b0)) ;72
                                stato <= #1 4'h5; $display(";A 74");		//(assert (= stato    0h5)) ;74
                            end
                        end
                        else begin
                            $display(";A 68");		//(assert (= (bv-comp rtr  0b1)   0b0)) ;68
                        end
                    end
                4'h5 :
                    begin
                        $display(";A 75");		//(assert (= stato    0h5)) ;75
                        if ((rts == 1'b0)) begin
                            $display(";A 76");		//(assert (= (bv-comp rts  0b0)   0b1)) ;76
                            ctr <= #1 1'b1; $display(";A 78");		//(assert (= ctr    0b1)) ;78
                            stato <= #1 4'h6; $display(";A 79");		//(assert (= stato    0h6)) ;79
                        end
                        else begin
                            $display(";A 77");		//(assert (= (bv-comp rts  0b0)   0b0)) ;77
                        end
                    end
                4'h6 :
                    begin
                        $display(";A 80");		//(assert (= stato    0h6)) ;80
                        if ((rts == 1'b1)) begin
                            $display(";A 81");		//(assert (= (bv-comp rts  0b1)   0b1)) ;81
                            voto0 <= #1 v_in[0]; $display(";A 83");		//(assert (= voto0    (bv-extract 0 0 v_in ))) ;83
                            voto1 <= #1 v_in[1]; $display(";A 84");		//(assert (= voto1    (bv-extract 1 1 v_in ))) ;84
                            voto2 <= #1 v_in[2]; $display(";A 85");		//(assert (= voto2    (bv-extract 2 2 v_in ))) ;85
                            voto3 <= #1 v_in[3]; $display(";A 86");		//(assert (= voto3    (bv-extract 3 3 v_in ))) ;86
                            ctr <= #1 1'b0; $display(";A 87");		//(assert (= ctr    0b0)) ;87
                            stato <= #1 4'h7; $display(";A 88");		//(assert (= stato    0h7)) ;88
                        end
                        else begin
                            $display(";A 82");		//(assert (= (bv-comp rts  0b1)   0b0)) ;82
                        end
                    end
                4'h7 :
                    begin
                        $display(";A 89");		//(assert (= stato    0h7)) ;89
                        if ((rtr == 1'b0)) begin
                            $display(";A 90");		//(assert (= (bv-comp rtr  0b0)   0b1)) ;90
                            cts <= #1 1'b0; $display(";A 92");		//(assert (= cts    0b0)) ;92
                            stato <= #1 4'h4; $display(";A 93");		//(assert (= stato    0h4)) ;93
                        end
                        else begin
                            $display(";A 91");		//(assert (= (bv-comp rtr  0b0)   0b0)) ;91
                        end
                    end
                4'h8 :
                    begin
                        $display(";A 94");		//(assert (= stato    0h8)) ;94
                        if ((rtr == 1'b0)) begin
                            $display(";A 95");		//(assert (= (bv-comp rtr  0b0)   0b1)) ;95
                            cts <= #1 1'b0; $display(";A 97");		//(assert (= cts    0b0)) ;97
                            stato <= #1 4'h1; $display(";A 98");		//(assert (= stato    0h1)) ;98
                        end
                        else begin
                            $display(";A 96");		//(assert (= (bv-comp rtr  0b0)   0b0)) ;96
                        end
                    end
                4'h9 :
                    begin
                        $display(";A 99");		//(assert (= stato    0h9)) ;99
                        voto0 <= #1 v_in[0]; $display(";A 100");		//(assert (= voto0    (bv-extract 0 0 v_in ))) ;100
                        voto1 <= #1 v_in[1]; $display(";A 101");		//(assert (= voto1    (bv-extract 1 1 v_in ))) ;101
                        voto2 <= #1 v_in[2]; $display(";A 102");		//(assert (= voto2    (bv-extract 2 2 v_in ))) ;102
                        voto3 <= #1 v_in[3]; $display(";A 103");		//(assert (= voto3    (bv-extract 3 3 v_in ))) ;103
                        sign <= #1 4'h8; $display(";A 104");		//(assert (= sign    0h8)) ;104
                        if (((((voto0 == 1'b1) && (voto1 == 1'b1)) && (voto2 == 1'b1)) && (voto3 == 1'b1))) begin
                            $display(";A 105");		//(assert (= (bv-and (bv-and (bv-and (bv-comp voto0  0b1) (bv-comp voto1  0b1)) (bv-comp voto2  0b1)) (bv-comp voto3  0b1))   0b1)) ;105
                            stato <= #1 4'hA; $display(";A 107");		//(assert (= stato    0hA)) ;107
                        end
                        else begin
                            $display(";A 106");		//(assert (= (bv-and (bv-and (bv-and (bv-comp voto0  0b1) (bv-comp voto1  0b1)) (bv-comp voto2  0b1)) (bv-comp voto3  0b1))   0b0)) ;106
                        end
                    end
                4'hA :
                    begin
                        $display(";A 108");		//(assert (= stato    0hA)) ;108
                        voto0 <= #1 (1'b1 ^ sign[0]); $display(";A 109");		//(assert (= voto0    (bv-xor 0b1 (bv-extract 0 0 sign )))) ;109
                        voto0 <= #1 (1'b0 ^ sign[1]); $display(";A 110");		//(assert (= voto0    (bv-xor 0b0 (bv-extract 1 1 sign )))) ;110
                        voto0 <= #1 (1'b0 ^ sign[2]); $display(";A 111");		//(assert (= voto0    (bv-xor 0b0 (bv-extract 2 2 sign )))) ;111
                        voto0 <= #1 (1'b1 ^ sign[3]); $display(";A 112");		//(assert (= voto0    (bv-xor 0b1 (bv-extract 3 3 sign )))) ;112
                        stato <= #1 4'h4; $display(";A 113");		//(assert (= stato    0h4)) ;113
                    end
            endcase
        end
        // Displaying module variables
        begin
            $display(";R ctr = %b", ctr);
            $display(";R cts = %b", cts);
            $display(";R last_g = %b", last_g);
            $display(";R last_r = %b", last_r);
            $display(";R sign = %b", sign);
            $display(";R stato = %b", stato);
            $display(";R v_out = %b", v_out);
            $display(";R voto0 = %b", voto0);
            $display(";R voto1 = %b", voto1);
            $display(";R voto2 = %b", voto2);
            $display(";R voto3 = %b", voto3);
        end
    end

endmodule

