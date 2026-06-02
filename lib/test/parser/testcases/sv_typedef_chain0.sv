module sv_typedef_chain0
  (output [3:0] out);

  typedef logic [3:0] base_t;
  typedef base_t alias_t;
  typedef mypkg::data_t imported_t;

  alias_t x;

  assign out = x;

endmodule
