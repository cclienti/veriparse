module loop_unrolling_repeat_continue0
  (input [3:0] skip, output reg [7:0] o);

   integer c;

   always @* begin
      o = 0;
      c = 0;
      repeat (4) begin : lp
         c = c + 1;
         if (c == skip) continue;
         o = o + 1;
      end
   end

endmodule
