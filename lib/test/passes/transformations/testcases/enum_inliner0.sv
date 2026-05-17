module enum_inliner0(input logic clk, input logic rst);
  typedef enum logic [1:0] {IDLE, RUN, DONE, ERROR} state_t;
  logic [1:0] state;
  always_ff @(posedge clk) begin
    if (state == IDLE) state <= RUN;
    else if (state == RUN) state <= DONE;
    else state <= IDLE;
  end
endmodule
