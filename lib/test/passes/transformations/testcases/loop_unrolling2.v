module loop_unrolling2;

   generate
      genvar i;
      for (i = 0; i < 10; i = i + 1) begin: gen1
         genvar j;
         for (j = i; j >= 1; j = j - 1) begin: gen2
            reg [i:0] R;
            initial begin
               R = i;
               $display("%m", R);
            end
         end
      end
   endgenerate

endmodule
