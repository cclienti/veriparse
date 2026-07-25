// Member selects normalize against the member's DECLARED range: a
// non-zero-lsb member ([7:4]) and an ascending member ([0:3]) index by
// their declared bit names, not zero-based offsets (§7.4.2).
module struct_range0(input logic [7:0] a, output logic nz_bit,
                     output logic [1:0] nz_part, output logic asc_msb,
                     output logic [1:0] asc_part);

  struct packed {
    logic [7:4] f;    // occupies [7:4]; f[5] is vector bit 5
    logic [0:3] g;    // ascending: g[0] is the member MSB, vector bit 3
  } s;

  always @(*) s = a;

  assign nz_bit = s.f[5];
  assign nz_part = s.f[6:5];
  assign asc_msb = s.g[0];
  assign asc_part = s.g[1:2];

endmodule
