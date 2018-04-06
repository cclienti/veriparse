module net0
  #(parameter integer WIDTH = 8)
  (input wire             reset,
   input wire             clk,
   input wire [WIDTH-1:0] in,
   output reg             out);

   wire                    a;
   wire signed [WIDTH-1:0] b, x;
   wire [WIDTH-1:0]        c [2**WIDTH-1:0], h [63:0] [31:0];
   wire [WIDTH-1:0]        d = in, e = in + in, f;

endmodule
