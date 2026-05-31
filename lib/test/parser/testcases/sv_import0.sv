module sv_import0
  (input  wire clk,
   output wire out);

  import sv_package0::*;
  import sv_package0::WIDTH;

  assign out = clk;

endmodule
