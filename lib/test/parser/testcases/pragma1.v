(* my_pragma *)
module pragma1(reset, clk, in, out);
   input wire       reset, clk;
   input wire [7:0] in;
   output reg       out;

   reg [7:0] ram [0:1023];
endmodule
