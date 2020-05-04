module loop_unrolling3;

   function automatic integer inc(input integer value);
      begin
         inc = value + 1;
      end
   endfunction

   function automatic integer dec(input integer value);
      begin
         dec = value - 1;
      end
   endfunction

   function automatic integer less(input integer a, b);
      begin
         less = a < b;
      end
   endfunction

   genvar i, j;
   generate
      for (i = 0; less(i, 10); i = inc(i)) begin
         for (j = 10-i; j >= 1; j = dec(j)) begin
            initial $display("%m", i*j);
         end
      end
   endgenerate

endmodule
