// Following code segment is generated from /home/meng/Code/concolic-testing/test/testcase/src/testcase.v:1
module top(clock, reset, in, out, __obs);
    input clock;
    input reset;
    input [7:0] in;
    output out;
    input __obs;

    reg out = 1'b0;
    reg [1:0] state = 2'b0;

    // Following code segment is generated from /home/meng/Code/concolic-testing/test/testcase/src/testcase.v:10
    always @(posedge clock) begin
        if ((reset == 1'b1)) begin
            state <= #1 2'b00; $display(";A 2");		//(assert (= state    0b00)) ;2
            out <= #1 1'b0; $display(";A 3");		//(assert (= out    0b0)) ;3
        end
        else begin
            if ((in == 8'h1F)) begin
                $display(";A 4");		//(assert (= (bv-comp in  0h1F)   0b1)) ;4
                state <= #1 2'b01; $display(";A 6");		//(assert (= state    0b01)) ;6
            end
            else begin
                $display(";A 5");		//(assert (= (bv-comp in  0h1F)   0b0)) ;5
                if (((in == 8'hB2) && (state == 2'b01))) begin
                    $display(";A 7");		//(assert (= (bv-and (bv-comp in  0hB2) (bv-comp state  0b01))   0b1)) ;7
                    state <= #1 2'b10; $display(";A 9");		//(assert (= state    0b10)) ;9
                end
                else begin
                    $display(";A 8");		//(assert (= (bv-and (bv-comp in  0hB2) (bv-comp state  0b01))   0b0)) ;8
                    if (((in == 8'h3C) && (state == 2'b10))) begin
                        $display(";A 10");		//(assert (= (bv-and (bv-comp in  0h3C) (bv-comp state  0b10))   0b1)) ;10
                        out <= #1 1'b1; $display(";A 12");		//(assert (= out    0b1)) ;12
                    end
                    else begin
                        $display(";A 11");		//(assert (= (bv-and (bv-comp in  0h3C) (bv-comp state  0b10))   0b0)) ;11
                        state <= #1 2'b00; $display(";A 13");		//(assert (= state    0b00)) ;13
                    end
                end
            end
        end
        // Displaying module variables
        begin
            $display(";R out = %b", out);
            $display(";R state = %b", state);
        end
    end

endmodule

