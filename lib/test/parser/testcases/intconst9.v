module intconst9
  (input wire       reset,
   input wire       clk,
   input wire [31:0] in,
   output reg [31:0] out);

   always @(posedge clk) out <= 32'hffff_ffff;

endmodule
