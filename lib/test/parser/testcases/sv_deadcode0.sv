module sv_deadcode0
  (input  logic       clk,
   input  logic       rst_n,
   input  logic [7:0] d,
   output logic [7:0] q);

  logic [7:0] unused_wire;

  always_ff @(posedge clk or negedge rst_n) begin
    if (!rst_n)
      q <= 8'h00;
    else
      q <= d;
  end

endmodule
