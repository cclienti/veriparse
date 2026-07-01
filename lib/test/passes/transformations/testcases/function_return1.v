module function_return1();

   function integer g(input integer x);
      if (x < 0) return 0;
      return x * 2;
   endfunction

   initial begin
      $display(g(-3));
   end

endmodule
