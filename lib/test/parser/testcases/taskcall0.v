module taskcall0
  #(parameter integer WIDTH = 8)
  (input wire             reset,
   input wire             clk,
   input wire [WIDTH-1:0] in0,
   input wire [WIDTH-1:0] in1,
   input wire [WIDTH-1:0] in2,
   input wire [WIDTH-1:0] in3);

   always @(posedge clk) mytask({in1, in2}, in0, otherfunc(in3));

endmodule
