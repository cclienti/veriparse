module function_dflt0();

   function automatic integer f(input integer a, input integer b = 3);
      return a + b;
   endfunction

   initial begin
      $display(f(1));
   end

endmodule
