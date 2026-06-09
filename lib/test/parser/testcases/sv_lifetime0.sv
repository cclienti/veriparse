module sv_lifetime0;

  // [lifetime] qualifier on a data declaration: static / automatic (IEEE
  // 1800-2017 §6.21, A.2.1.3). At module scope variables are static-lifetime by
  // default, so the keyword is explicit-but-legal; it stacks with const/var. The
  // lifetime is carried by DataModifier.lifetime. (`static` here is a lifetime,
  // not C++ class-static: each module instance keeps its own copy.)
  static int       a = 2;     // DataModifier{lifetime:STATIC} over Int
  automatic int    b;         // DataModifier{lifetime:AUTOMATIC} over Int
  var static int   c;         // var + static stack (canonical [var][lifetime])
  const static int d = 5;     // const + static stack

endmodule
