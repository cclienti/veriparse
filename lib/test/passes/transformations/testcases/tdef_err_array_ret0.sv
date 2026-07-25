// An array typedef as a function return type is legal source (A.2.6), but
// inlining the alias would leave an unpacked array type with no anonymous
// syntax to render: rejected loudly rather than silently losing the shape.
module tdef_err_array_ret0(input logic [7:0] a, output logic [7:0] b);
  typedef logic [7:0] mem_t [4];
  function automatic mem_t make(input logic [7:0] v);
    make[0] = v;
  endfunction
  assign b = a;
endmodule
