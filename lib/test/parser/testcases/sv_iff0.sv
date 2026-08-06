module sv_iff0 (input logic clk, input logic rst, input logic en,
                input logic d, output logic q, output logic r, output logic s);

   always_ff @(posedge clk iff en) q <= d;

   always_ff @(posedge clk iff en == 1'b1 or posedge rst) begin
      if (rst) r <= 1'b0;
      else r <= d;
   end

   initial begin
      s <= 1'b0;
      @(negedge clk iff ~en);
      s <= 1'b1;
   end
endmodule
