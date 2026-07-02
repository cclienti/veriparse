module vf_while_nonconst0
  (input [7:0] gb);

   integer i, f, y;

   initial begin
      f = 1;
      i = 0;
      while (i < gb) begin
         f = 9;
         i = i + 1;
      end
      y = f;
   end

endmodule
