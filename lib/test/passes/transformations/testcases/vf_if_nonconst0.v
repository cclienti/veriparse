module vf_if_nonconst0
  (input g);

   integer x, y;

   initial begin
      x = 0;
      if (g) x = 1;
      y = x;
   end

endmodule
