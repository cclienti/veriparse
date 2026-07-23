// A subroutine-local typedef does not escape its body (§6.18): referencing
// it at module scope is an error.
module tdef_err_tf_leak0(input logic [3:0] a, output logic [3:0] b);

  function automatic logic [3:0] tfl_id(input logic [3:0] v);
    typedef logic [3:0] local_t;
    local_t s;
    s = v;
    return s;
  endfunction

  local_t x;
  assign x = tfl_id(a);
  assign b = x;

endmodule
