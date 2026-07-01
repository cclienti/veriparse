module function_continue0();

   function automatic integer f(input integer n);
      integer i;
      begin
         f = 0;
         for (i = 0; i < n; i = i + 1) begin
            if (i == 2) continue;
            f = f + 1;
         end
      end
   endfunction

   initial begin
      $display(f(5));
   end

endmodule
