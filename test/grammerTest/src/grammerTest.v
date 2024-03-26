module grammerTest(
    input   clk, 
    input   reset, 
    input   [31:0] in, 
    input   [3:0]  count1,
    input   [3:0]  count,
    input          register,
    output  reg [31:0] out,
    output  reg [31:0] out1
);

  reg [31:0] myArray [0:3]; // 4-element array of 8-bit elements
  reg [1:0]  addr;
  reg [31:0] temp;
  reg [7:0] cnt;
  reg [31:0] temp1;


always @(posedge clk ) begin
  if (reset == 1'b1) begin
    temp1 <= 1'b0;
  end
end

always @(posedge clk ) begin
  if (reset == 1'b1) begin
    addr <= 2'b0;
    temp <= 32'b0;
    cnt <= 8'b0;
  end
  else begin
    addr <= addr + 1'b1;
    temp <= in;
    cnt <= cnt + 1'b1;
  end
end

always @(posedge clk) begin
  if (!cnt ) begin
    case (addr)
      2'b00: 
      begin
            myArray[0] <= temp%5;
            addr <= addr + 1'b1;
      end
      2'b01: 
            myArray[1] <= temp**5;
      2'b10: 
            myArray[2] <= temp**2;
      2'b11: 
            myArray[3] <= temp%5;
  endcase
  end
  else if (cnt < 8'b1000_0000) begin
    case (addr)
      2'b00: myArray[0] <= temp/2;
      2'b01: myArray[1] <= temp/2;
      2'b10: myArray[2] <= temp/2;
      2'b11: myArray[3] <= temp/2;
  endcase
  end
  else if (cnt < 8'b1100_0000) begin
    case (addr)
      2'b00: myArray[0] <= temp>>2;
      2'b01: myArray[1] <= temp>>2;
      2'b10: myArray[2] <= temp>>2;
      2'b11: myArray[3] <= temp>>2;
      endcase
  end
  else begin
    case (addr)
      2'b00: myArray[0] <= 32'b0;
      2'b01: myArray[1] <= 32'b0;
      2'b10: myArray[2] <= 32'b0;
      2'b11: myArray[3] <= 32'b0;
    endcase
  end
end



endmodule
