// Packed struct fundamentals (§7.2.1): member offsets (first member at the
// MSBs), whole-struct assignment, member write/read, member bit/part
// selects, and a nested packed struct.
module struct_basic0(input logic [7:0] a, output logic [7:0] y,
                     output logic [3:0] hi, output logic bit2,
                     output logic [1:0] mid, output logic [2:0] tag);

  typedef struct packed {
    logic [3:0] hi;   // [7:4]
    logic [3:0] lo;   // [3:0]
  } byte_t;

  typedef struct packed {
    logic [2:0] tag;  // [7:5]
    struct packed {
      logic [1:0] x;  // [4:3]
      logic [2:0] q;  // [2:0]
    } body;
  } pkt_t;

  byte_t b;
  byte_t c;
  pkt_t p;

  always @(*) begin
    b.hi = a[7:4];
    b.lo = a[3:0];
    c = b;                 // whole-struct assign: plain vector copy
    p = {a[7:5], a[4:0]};
  end

  assign y = c;
  assign hi = b.hi;
  assign bit2 = b.lo[2];   // bit-select of a member -> folded offset
  assign mid = b.hi[2:1];  // part-select of a member -> folded offsets
  assign tag = p.tag;

endmodule
