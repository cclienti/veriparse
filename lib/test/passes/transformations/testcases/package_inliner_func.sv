package P;

  localparam W = 8;
  typedef enum logic [1:0] {IDLE, RUN, DONE} state_t;
  function integer dbl;
    input integer x;
    dbl = x * 2;
  endfunction

endpackage

module package_inliner_func
  (output [7:0] dout);

  import P::*;

  state_t st;
  localparam K = dbl(W);

  assign dout = K + st;

endmodule
