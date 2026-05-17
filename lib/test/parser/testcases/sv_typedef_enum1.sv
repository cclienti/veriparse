module sv_typedef_enum1
  (input  logic clk,
   output logic [1:0] out);

  typedef enum logic [1:0] {S0, S1, S2, S3} fsm_t;

  assign out = 2'h0;

endmodule
