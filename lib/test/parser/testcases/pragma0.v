/* pragma0 */
module pragma0(reset, clk, in, out);
   input wire       reset, clk;
   input wire [7:0] in;
   output reg       out;

   (* max_depth = 512 *) reg [7:0] ram[0:1023];
endmodule
