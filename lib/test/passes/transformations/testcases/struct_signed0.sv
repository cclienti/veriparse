// Signed members re-wrap in signed'() in expression position (§11.5.1
// makes a bare part-select unsigned); assignment targets stay bare.
module struct_signed0(input logic [7:0] a, output logic [8:0] sum,
                      output logic ge);

  struct packed {
    logic signed [7:0] s;
    byte               b;   // byte is signed by default (§6.11)
  } v;

  always @(*) begin
    v.s = a;
    v.b = ~a;
  end

  assign sum = v.s + v.b;          // signed'(v[15:8]) + signed'(v[7:0])
  assign ge = v.s >= -8'sd1;

endmodule
