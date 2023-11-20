// Following code segment is generated from /home/meng/Code/concolic-testing/test/grammerTest/src/grammerTest.v:1
module grammerTest(clk, reset, in, out, __obs);
    input clk;
    input reset;
    input in;
    output [7:0] out;
    input __obs;

    reg [7:0] cnt = 8'b0;
    reg [7:0] out = 8'b0;

    // Following code segment is generated from /home/meng/Code/concolic-testing/test/grammerTest/src/grammerTest.v:10
    always @(posedge reset or posedge clk) begin
        if (reset) begin
            cnt <= #1 8'h00; $display(";A 2");		//(assert (= cnt    0h00)) ;2
        end
        else begin
            if ((in == 1'b0)) begin
                $display(";A 3");		//(assert (= (bv-comp in  0b0)   0b1)) ;3
                cnt <= #1 (cnt + 8'h01); $display(";A 5");		//(assert (= cnt    (bv-add cnt  0h01))) ;5
            end
            else begin
                $display(";A 4");		//(assert (= (bv-comp in  0b0)   0b0)) ;4
            end
        end
    end

    // Following code segment is generated from /home/meng/Code/concolic-testing/test/grammerTest/src/grammerTest.v:19
    always @(posedge reset or posedge clk) begin
        if (reset) begin
            out <= #1 8'h00; $display(";A 8");		//(assert (= out    0h00)) ;8
        end
        else begin
            if ((cnt < 8'h04)) begin
                $display(";A 9");		//(assert (= (bool-to-bv (bv-lt cnt  0h04))   0b1)) ;9
                out <= #1 8'h00; $display(";A 11");		//(assert (= out    0h00)) ;11
            end
            else begin
                $display(";A 10");		//(assert (= (bool-to-bv (bv-lt cnt  0h04))   0b0)) ;10
                if ((cnt <= 8'h10)) begin
                    $display(";A 12");		//(assert (= (bool-to-bv (bv-le cnt  0h10))   0b1)) ;12
                    out <= #1 (cnt ** 8'h02); $display(";A 14");		//(assert (= out    (bv-pow cnt  0h02))) ;14
                end
                else begin
                    $display(";A 13");		//(assert (= (bool-to-bv (bv-le cnt  0h10))   0b0)) ;13
                    out <= #1 (cnt / 8'h02); $display(";A 15");		//(assert (= out    (bv-div cnt  0h02))) ;15
                end
            end
        end
    end

endmodule

