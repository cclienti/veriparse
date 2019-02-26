module scope0;

   genvar i;

   generate
      for (i=0; i<5; i=i+1) begin: FOR_1
         integer x=i;
      end
   endgenerate

   generate
      for (i=0; i<5; i=i+1) begin: FOR_2
         always @(*) begin
            $display(FOR_1[i].x);
         end
      end
   endgenerate

endmodule
