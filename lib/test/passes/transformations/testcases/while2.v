module while2;

   integer i, j, k;

   initial begin
      i = 0;
      while(i<=3) begin
         j = 0;
         while(j<=i) begin
            k = 0;
            while(k <= j) begin
               $display("while i=%0d j=%0d k=%0d", i, j, k);
               k = k + 1;
            end
            j = j + 1;
         end
         i = i + 1;
      end
   end

endmodule
