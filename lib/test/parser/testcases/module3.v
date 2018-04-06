/* module3 */
module module3
  #(parameter [7:0] T1, T2=3,
    parameter integer T4=2)
   (reset, clk, in, out);

   parameter integer A = 2, B = 3, C;
   parameter signed X, Y;
   parameter Z;

   input wire       reset, clk;
   input wire [7:0] in;
   output reg       out;
endmodule
