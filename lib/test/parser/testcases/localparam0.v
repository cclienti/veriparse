module localparam0
  #(parameter integer WIDTH = 8)
  (input wire             reset,
   input wire             clk,
   input wire [WIDTH-1:0] in,
   output reg             out);

   localparam integer DOUBLE_WIDTH = WIDTH * 2;
   localparam [5:0] [2:0] MYLOCALPARAM1 = 18'd1, MYLOCALPARAM2 = 18'b10101;

endmodule
