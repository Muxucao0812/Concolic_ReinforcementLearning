module top(clock, reset, in, out);

input clock, reset;
input [7:0] in;
output reg out;

parameter [1:0] A=2'b00, B=2'b01, C=2'b10;
reg [1:0] state;

always @(posedge clock) begin
  if (reset == 1) begin
    state <= A;
    out <= 1'b0;
  end
  else if (in == 8'h1F) 
    state <= B;
  else if ((in == 8'hB2) && (state == B))
    state <= C;
  else if ((in == 8'h3C) && (state == C))
    out <= 1'b1;
  else
    state <= A;
end


endmodule
