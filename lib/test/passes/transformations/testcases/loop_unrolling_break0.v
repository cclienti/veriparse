module loop_unrolling_break0
  (input [3:0] sel, output reg [7:0] o);

   integer i;

   always @* begin
      o = 0;
      for (i = 0; i < 4; i = i + 1) begin : lp
         if (i == sel) break;
         o = o + 1;
      end
   end

endmodule
