module function_break0();

   function automatic integer f(input integer n);
      integer i;
      begin
         f = 0;
         for (i = 0; i < 100; i = i + 1) begin
            if (i > n) break;
            f = f + i;
         end
      end
   endfunction

   initial begin
      $display(f(3));
   end

endmodule
