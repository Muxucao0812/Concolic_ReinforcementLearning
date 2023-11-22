// Following code segment is generated from /home/meng/Code/concolic-testing/test/grammerTest/src/grammerTest.v:1
module grammerTest(clk, reset, in, out, sig_display, __obs);
    input clk;
    input reset;
    input [31:0] in;
    output [31:0] out;
    input sig_display;
    input __obs;

    reg [1:0] addr = 2'b0;
    reg [7:0] cnt = 8'b0;
    reg [31:0] myArray [0:3] = 32'b0;
    reg [31:0] out = 32'b0;
    reg [31:0] temp = 32'b0;

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
                        myArray