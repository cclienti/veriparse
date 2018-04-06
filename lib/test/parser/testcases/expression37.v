module expression37
  #(parameter integer WIDTH = 8)
  (input wire               reset,
   input wire               clk,
   input wire [WIDTH-1:0]   in,
   output reg [2*WIDTH-1:0] out);

   always @(posedge clk) {out[2*WIDTH-1 -: WIDTH], out[0 +: WIDTH]} <= {2{in}};

endmodule
