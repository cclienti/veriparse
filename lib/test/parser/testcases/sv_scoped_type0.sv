module sv_scoped_type0
  (output [7:0] out);

  mypkg::data_t sig;
  mypkg::word_t a, b;

  assign out = sig;

endmodule
