module functioncall0
  #(parameter integer WIDTH = 8)
  (input wire             reset,
   input wire             clk,
   input wire [WIDTH-1:0] in0,
   input wire [WIDTH-1:0] in1,
   input wire [WIDTH-1:0] in2,
   input wire [WIDTH-1:0] in3,
   output reg [WIDTH-1:0] out);

   always @(posedge clk) out <= myfunc(in0, {in1, in2}, otherfunc(in3));

endmodule
