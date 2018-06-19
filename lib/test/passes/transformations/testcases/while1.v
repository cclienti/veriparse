module while1;

   integer i, j;

   initial begin
      i = 0;
      while(i<=3) begin
         j = 0;
         while(j<=3) begin
            $display("while i=%0d j=%0d", i, j);
            j = j + 1;
         end
         i = i + 1;
      end
   end

endmodule
