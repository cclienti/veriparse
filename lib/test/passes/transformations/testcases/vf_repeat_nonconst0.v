module vf_repeat_nonconst0
  (input [7:0] gb);

   integer f, y;

   initial begin
      f = 1;
      repeat (gb) begin
         f = 9;
      end
      y = f;
   end

endmodule
