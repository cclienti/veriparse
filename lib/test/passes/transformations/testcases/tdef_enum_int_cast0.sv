// Bug repro (ADR-0009): a cast to a default-based enum typedef. With no
// explicit base the enum's base is `int` (IEEE 1800-2017 §6.19.3), so the
// alias lowers to a 32-bit signed int. The cast `e_t'(a)` is legal SV and
// must flatten to a size cast; substitute_typedef_cast wrongly rejects any
// non logic/bit alias, aborting the flatten.
module tdef_enum_int_cast0(input logic [7:0] a, output logic [7:0] b);
  typedef enum {EIC_A, EIC_B} e_t;
  assign b = e_t'(a);
endmodule
