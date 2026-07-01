module function_return0();

   function integer f(input integer x);
      if (x < 0) return 0;
      return x * 2;
   endfunction

   initial begin
      $display(f(5));
   end

endmodule
