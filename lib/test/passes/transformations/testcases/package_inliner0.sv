package mypkg;

  localparam WIDTH = 8;

  typedef logic [7:0] data_t;

  function data_t make;
    input [7:0] v;
    make = v;
  endfunction

endpackage

module package_inliner0
  (input  wire clk,
   output [7:0] dout);

  import mypkg::*;

  data_t value;
  localparam W = mypkg::WIDTH;

  assign dout = value;

endmodule
