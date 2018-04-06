module intconst4
  (input wire       reset,
   input wire       clk,
   input wire [7:0] in,
   output reg [7:0] out);

   always @(posedge clk) out <= 7 'sD 0;

endmodule
