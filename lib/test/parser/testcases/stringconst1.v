module stringconst1
  #(parameter MY_STRING = "Hello\nWorld!")
  (input wire        reset,
   input wire        clk,
   input wire [31:0] in,
   output reg [31:0] out);

   always @(posedge clk) out <= 1.0;

endmodule
