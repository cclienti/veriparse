module vf_loop_stale0;

   integer i, n, x, y;

   initial begin
      x = 5;
      for (i = 0; i < n; i = i + 1)
         x = x + 1;
      y = x;
   end

endmodule
