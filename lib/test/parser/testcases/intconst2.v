module intconst2
  (input wire       reset,
   input wire       clk,
   input wire [7:0] in,
   output reg [7:0] out);

   always @(posedge clk) out <= 8'h55;

endmodule
