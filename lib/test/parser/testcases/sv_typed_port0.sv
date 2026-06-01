module sv_typed_port0
  (input  my_t a, b,
   input  mypkg::data_t d,
   output mypkg::res_t q);

  assign q = a;

endmodule
