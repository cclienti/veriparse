module for1;

   integer i, j;

   initial begin
      for(i=0; i<=3; i=i+1)
         for(j = 0; j<=3; j = j + 1) begin
            $display("for i=%0d j=%0d", i, j);
         end
   end

endmodule
