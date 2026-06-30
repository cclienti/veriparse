package P;

  localparam x = 99;
  function integer f;
    input integer x;
    f = x + 1;
  endfunction

endpackage

module package_inliner_func_shadow
  (output [7:0] dout);

  import P::*;

  localparam R = f(3);

  assign dout = R;

endmodule
