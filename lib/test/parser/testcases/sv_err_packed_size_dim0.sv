module sv_err_packed_size_dim0;
  // illegal: a packed dimension on a named type must be a range [msb:lsb],
  // not a size [N] (IEEE 1800-2017 7.4.1, ADR-0001 §3.8).
  var my_t [4] x;
endmodule
