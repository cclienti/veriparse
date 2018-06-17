module repeat2;

   integer i, j, k;

   initial begin
      i = 0;
      repeat(3) begin
         i = i + 1;
         j = 0;
         repeat(i) begin
            j = j + 1;
            k = 0;
            repeat(j) begin
               k = k + 1;
               repeat(k) $display("repeat i=%0d j=%0d k=%0d", i, j, k);
            end
         end
      end
   end

endmodule
