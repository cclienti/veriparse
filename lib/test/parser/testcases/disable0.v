module disable0 ();

   initial begin
      integer i, j;

      for(i=0; i<9; i=i+1) begin: OUTER
         for(j=0; j<9; j=j+1) begin: INNER
            $display ("(i, j) = (%d, %d)", i, j);
            if (i==7) disable OUTER;
            if (j==5) disable INNER;
         end
      end

   end

endmodule
