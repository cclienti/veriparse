module sv_tf_arg_dims_err0;

  typedef logic [1:0] pair_t;

  function logic f(pair_t a [2]);
    return a[0][0];
  endfunction

endmodule
