// Indexed part-selects on a member fold into the member offset: the
// output must never stack a select onto a part-select expression.
module struct_idxsel0(input logic [7:0] a, input logic [2:0] i,
                      output logic [1:0] up, output logic [1:0] down);

  struct packed {
    logic [7:0] f;    // [15:8]
    logic [7:0] lo;   // [7:0]
  } s;

  always @(*) s = {a, ~a};

  assign up = s.f[i +: 2];
  assign down = s.f[i -: 2];

endmodule
