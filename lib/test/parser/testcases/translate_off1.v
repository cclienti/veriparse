module translate_off1
  #(parameter integer WIDTH = 8)
  (input wire             reset,
   input wire             clk,
   input wire [WIDTH-1:0] in,
   output reg             out);

   localparam integer DOUBLE_WIDTH = WIDTH * 2;

   /* synthesis translate_off */
   localparam [5:0] MYLOCALPARAM1 = 5'b00001, MYLOCALPARAM2 = 5'b10101;
   wire [MYLOCALPARAM1-1:0] simu_wire;
   /*synthesis translate_on*/

   reg [DOUBLE_WIDTH-1:1] rtl_reg;

endmodule
