module constant_folding2
  #(parameter X = myfunc(42));

   function integer myfunc(input integer value);
      begin
         myfunc = value * 2;
      end
   endfunction

   initial begin
      $display("X=%0d", X);
   end

endmodule
