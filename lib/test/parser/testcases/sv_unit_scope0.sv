module sv_unit_scope0
  (input  $unit::data_t din,
   output [7:0] dout);

  localparam W = $unit::WIDTH;

  assign dout = $unit::mask(din);

endmodule
