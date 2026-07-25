// A struct-typed function return: the return type lowers to a vector and
// the implicit function-name variable resolves member writes.
module struct_func_ret0(input logic [7:0] a, output logic [7:0] y);

  typedef struct packed {
    logic [3:0] hi;
    logic [3:0] lo;
  } byte_t;

  function automatic byte_t make(input logic [7:0] v);
    make.hi = v[3:0];
    make.lo = v[7:4];
  endfunction

  assign y = make(a);

endmodule
