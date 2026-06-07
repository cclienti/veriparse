module sv_signing0;

  // Integer atom types are SIGNED by default: `unsigned` is the spelled form,
  // a redundant `signed` is dropped on round-trip (int signed -> int).
  int          a;            // signed (default)   -> `int a;`
  int unsigned b;            // unsigned            -> `int unsigned b;`
  byte unsigned c;
  longint unsigned d;

  // Integer vector types are UNSIGNED by default: `signed` is the spelled form,
  // a redundant `unsigned` is dropped on round-trip (bit unsigned -> bit).
  bit          [3:0] e;      // unsigned (default)  -> `bit [3:0] e;`
  bit signed   [3:0] f;      //                     -> `bit signed [3:0] f;`
  bit unsigned [3:0] g;      // explicit == default -> `bit [3:0] g;`

  // $unsigned / $signed system functions keep working (unsigned is now a keyword).
  localparam U = $unsigned(-1);
  localparam S = $signed(8'hFE);

endmodule
