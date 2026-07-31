// Data-declaration lifetimes (IEEE 1800-2017 §6.21): `static`/`automatic` on a
// variable, alone or with `var`/`const`. Kept as its own testcase so the
// module-header lifetime work cannot displace this coverage.
module sv_lifetime1;
   static int a = 2;
   automatic int b = 3;
   var static int c = 4;
   const static int d = 5;
endmodule
