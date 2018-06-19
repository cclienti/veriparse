module while0;

   integer i;

   initial begin
      i = 0;
      while(i < 5) begin
         $display("while i=%0d", i);
         i = i + 1;
      end
   end

endmodule
