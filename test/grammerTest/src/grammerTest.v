module grammerTest(
    input clk, 
    input reset, 
    input in, 
    output reg [7:0] out
);

reg [7:0] cnt;

always @(posedge clk or posedge reset) begin
    if (reset) begin
        cnt <= 8'b0;
    end
    else if (in == 1'b0) begin
        cnt <= cnt + 8'b1;
    end
end

always @(posedge clk or posedge reset)
begin
    if (reset)
    begin
        out <= 8'b0; //在复位时，输出为0
    end
    else
    begin
        if (cnt < 8'b0000_0100) begin
            out <= 8'b0; //如果输入是0，则输出错误码
        end
        else if (cnt <= 8'b0001_0000) begin
            out <= cnt ** 2; //否则，输出输入除以3的余数
        end
        else begin
            out <= cnt / 2; 
        end
    end
end

endmodule