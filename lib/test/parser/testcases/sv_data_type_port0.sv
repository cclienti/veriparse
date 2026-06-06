module sv_data_type_port0
  (input  int          a,
   output bit  [3:0]   b,
   input  byte         c,
   output bit signed [7:0] d,
   input  shortint     e);

  assign b = a[3:0];

endmodule
