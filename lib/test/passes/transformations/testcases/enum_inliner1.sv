module enum_inliner1(input logic clk);
  typedef enum logic [2:0] {A = 2, B, C = 5, D} state_t;
  logic [2:0] x;
  assign x = B;
endmodule
