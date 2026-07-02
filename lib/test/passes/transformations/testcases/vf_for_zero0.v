module vf_for_zero0;

   integer i, f, y;

   initial begin
      f = 1;
      for (i = 0; i < 0; i = i + 1)
         f = 9;
      y = f;
   end

endmodule
