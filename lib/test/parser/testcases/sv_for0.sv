module sv_for0
  #(parameter int DEPTH = 8)
  (input  logic             clk,
   input  logic             rst_n,
   output logic [DEPTH-1:0] out);

  integer i;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
      for (int i = 0; i < DEPTH; i++) begin
        out[i] <= 1'b0;
      end
    end else begin
      for (int i = 0; i < DEPTH; i++) begin
        out[i] <= 1'b1;
      end
    end
  end

endmodule
