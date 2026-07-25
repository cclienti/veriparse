// A packed struct through a subroutine: a struct-typed ANSI arg lowers to a
// vector arg, and body member accesses rewrite in the function scope.
module struct_func0(input logic [7:0] a, output logic [7:0] y);

  typedef struct packed {
    logic [3:0] hi;
    logic [3:0] lo;
  } byte_t;

  function automatic logic [7:0] swap(input byte_t v);
    return {v.lo, v.hi};
  endfunction

  byte_t d;
  always @(*) d = a;
  assign y = swap(d);

endmodule
