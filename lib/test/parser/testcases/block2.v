module block2();
   parameter A=2;
   genvar i;

   generate
      for(i=0; i<5; i=i+1) begin: GEN_LOOP_1
         initial $display("GEN_LOOP_1: %0d ", i);
      end
   endgenerate

   generate
      for(i=0; i<5; i=i+1) begin: GEN_LOOP_2
         initial $display("GEN_LOOP_2: %0d ", i);
      end
   endgenerate

   generate
      if (A==2) begin: TEST
         initial $display("TEST: %0d ", 2);
      end
   endgenerate

   generate
      case(A)
         2: begin: CASE
            initial $display("CASE: %0d ", 2);
         end
      endcase
   endgenerate

   generate
      for(i=0; i<5; i=i+1) initial $display("inline for loop: %0d", i);
      initial $display("inline after for");
   endgenerate

endmodule
