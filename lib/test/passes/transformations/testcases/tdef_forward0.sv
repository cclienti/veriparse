// A forward typedef completed later in the same scope: references after the
// completing definition substitute normally.
module tdef_forward0(input logic [1:0] a, output logic [1:0] b);
  typedef enum st_t;
  typedef enum logic [1:0] {TF_A, TF_B} st_t;
  st_t s;
  assign s = st_t'(a);
  assign b = s;
endmodule
