module scope2;

   genvar i, j;

   generate
      for (i=0; i<5; i=i+1) begin: GEN_LOOP_1
         integer x=i;
      end
   endgenerate

   generate
      for (i=0; i<5; i=i+1) begin: GEN_LOOP_2
         always @(*) begin
            $display("x[%0d]=%0d", i, GEN_LOOP_1[i].x);
         end
      end
   endgenerate

   integer k;
   initial begin: ANOTHER
      for (k=0; k<5; k=k+1) begin: LOOP_1
         $display("k=%0d", k);
      end
   end

   integer l;
   initial begin
      for (l=0; l<5; l=l+1) begin: LOOP_2
         $display("l=%0d", l);
      end
   end

endmodule
