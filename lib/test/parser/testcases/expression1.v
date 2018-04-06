module expression1
  #(parameter integer WIDTH = 8)
  (input wire                    reset,
   input wire                    clk,
   input wire signed [WIDTH-1:0] in,
   output reg signed [WIDTH-1:0] out);

   always @(posedge clk) out <= +in;

endmodule
