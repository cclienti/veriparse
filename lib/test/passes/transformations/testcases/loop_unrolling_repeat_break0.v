module loop_unrolling_repeat_break0
  (input [3:0] sel, output reg [7:0] o);

   integer c;

   always @* begin
      o = 0;
      c = 0;
      repeat (4) begin : lp
         if (c == sel) break;
         o = o + 1;
         c = c + 1;
      end
   end

endmodule
