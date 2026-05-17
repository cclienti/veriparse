module sv_typedef_enum0
  (input  logic clk,
   output logic [1:0] state);

  typedef enum logic [1:0] {IDLE = 2'h0, RUN = 2'h1, DONE = 2'h2} state_t;

  assign state = IDLE;

endmodule
