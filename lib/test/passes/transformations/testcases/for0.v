module for0;

   integer i;

   initial begin
      for(i = 0; i < 5; i = i + 1)
        $display("for i=%0d", i);
      $display("after for i=%0d", i);
   end

endmodule
