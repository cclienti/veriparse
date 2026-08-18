// A zero-argument call carries no args list at all: constant evaluation
// must treat it as an empty one, not crash, and fold the call.
module resolve_zeroarg0(output logic [7:0] y);

  function logic [7:0] gv();
    return 8'h2A;
  endfunction

  assign y = gv();

endmodule
