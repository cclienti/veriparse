module sv_port_signing0
  // Implicit port type with explicit signing (no type keyword):
  //   implicit_data_type ::= [ signing ] { packed_dimension }  (IEEE 1800-2017)
  // The tri-state signing is preserved on the ImplicitType: `signed` -> SIGNED,
  // `unsigned` -> UNSIGNED, absent -> NONE (the default is resolved by a pass).
  (input  signed   [7:0] a,
   input  unsigned [7:0] b,
   input            [7:0] c,
   output unsigned [3:0] d);

  // Same on task/function arguments.
  function automatic logic f(input signed [3:0] x, input unsigned [3:0] y);
    f = x[0] ^ y[0];
  endfunction

endmodule
