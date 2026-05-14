module sv_pragma0
  (input  logic clk,
   input  logic [7:0] d,
   output logic [7:0] q);

  (* ram_style = "distributed" *)
  (* ramstyle  = "logic"       *)
  logic [7:0] mem [15:0];

  always_ff @(posedge clk) begin
    q <= mem[0];
  end

endmodule
