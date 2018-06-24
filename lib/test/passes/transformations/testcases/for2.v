module for2;

   integer i, j, k;

   initial begin
      for(i=0; i<=3; i=i+1)
         for(j = 0; j<=i; j=j+1) begin
            for(k = 0; k<=j; k=k+1) begin
               if(i*j*k != 1)
                 $display("for i=%0d j=%0d k=%0d", i, j, k);
            end
         end
   end

endmodule
