module loop_unrolling_repeat_nested0
  (output reg [7:0] o);

   integer i;

   always @* begin
      o = 0;
      repeat (2) begin : rp
         for (i = 0; i < 2; i = i + 1) begin : lp
            o = o + 1;
         end
      end
   end

endmodule
