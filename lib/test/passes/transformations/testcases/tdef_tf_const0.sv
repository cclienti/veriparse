// A constant call to a function whose body declares a local typedef: the
// pre-inline folding attempt leaves the call un-evaluated, and the second
// folding (after typedef inlining) resolves the dims to a constant.
module tdef_tf_const0(input logic [7:0] a, output logic [7:0] b);

  function automatic integer tfc_width(input integer n);
    typedef logic [31:0] acc_t;
    acc_t acc;
    acc = n * 2;
    return acc;
  endfunction

  logic [tfc_width(4)-1:0] x;
  assign x = a;
  assign b = x[7:0];

endmodule
