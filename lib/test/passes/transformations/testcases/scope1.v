module scope1;

   genvar i, j;

   generate
      for (i=0; i<5; i=i+1) begin: FOR_A
         for (j=0; j<5; j=j+1) begin: FOR_B
            integer x=i*j;
         end
      end
   endgenerate

   generate
      for (i=0; i<5; i=i+1) begin: FOR_C
         for (j=0; j<5; j=j+1) begin: FOR_D
            always @(*) begin
               $display("%1d * %1d = %2d", i, j, FOR_A[i].FOR_B[j].x);
            end
         end
      end
   endgenerate

endmodule
