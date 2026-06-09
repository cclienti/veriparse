module sv_var0;

  // SystemVerilog data declaration qualifiers (IEEE 1800-2017 §6.8, A.2.1.3):
  //   data_declaration ::= [const] [var] [lifetime] data_type_or_implicit ... ;
  // A bare declaration stays a plain data-type node; a qualified one is wrapped
  // in a DataModifier above its data type. With no type keyword the type is
  // implicit (logic) and modelled by an ImplicitType node.
  int           a;        // bare: plain Int, no DataModifier
  var int       b;        // DataModifier{is_var} over Int
  var           c = 2;    // implicit type -> ImplicitType, with initializer
  var [3:0]     d;        // implicit type with packed dimensions
  var signed    e;        // implicit type, signed
  const int     f = 5;    // DataModifier{is_const} over Int
  const var int g = 7;    // qualifiers stack on one DataModifier (const needs init)
  reg           h;        // bare: plain Reg, unchanged

endmodule
