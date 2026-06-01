module sv_typedef_alias0
  (output [3:0] out);

  typedef logic [3:0] nib_t;
  typedef logic bit_t;

  nib_t a, b;
  bit_t c;

  assign out = a;

endmodule
