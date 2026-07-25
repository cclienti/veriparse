// A packed union overlays equal-width members on the full vector (§7.3.1).
module struct_union0(input logic [7:0] a, output logic [7:0] whole,
                     output logic [3:0] hi);

  union packed {
    logic [7:0] raw;
    struct packed {
      logic [3:0] hi;
      logic [3:0] lo;
    } split;
  } u;

  always @(*) u.raw = a;

  assign whole = u.raw;
  assign hi = u.split.hi;

endmodule
