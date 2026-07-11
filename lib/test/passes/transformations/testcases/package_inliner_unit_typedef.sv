typedef logic [3:0] nib_t;

// A module-local typedef of the same name shadows the unit-scope one
// (IEEE 1800-2017 §23.9): m2's t is [7:0], not the unit-scope [3:0].
typedef logic [3:0] t;

module m1(input logic [3:0] a, output logic [3:0] b);
  nib_t x;
  assign x = a;
  assign b = x;
endmodule

module m2(input logic [7:0] a, output logic [7:0] b);
  typedef logic [7:0] t;
  t y;
  assign y = a;
  assign b = y;
endmodule
