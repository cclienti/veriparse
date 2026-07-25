// An array of packed structs: element member access routes the index
// through the lowered vector (`bank[i].f` -> `bank[i][msb:lsb]`).
module struct_array0(input logic [7:0] a, output logic [3:0] y0, output logic [3:0] y1);

  typedef struct packed {
    logic [3:0] hi;
    logic [3:0] lo;
  } byte_t;

  byte_t bank [2];

  always @(*) begin
    bank[0] = a;
    bank[1] = {a[3:0], a[7:4]};
  end

  assign y0 = bank[0].hi;
  assign y1 = bank[1].lo;

endmodule
