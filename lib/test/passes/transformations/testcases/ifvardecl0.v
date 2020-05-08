module ifvardecl0;

   initial begin
      if (1) begin
         integer x = 1;
         $display("x = %0d", x);
      end
      else begin
         integer x = 1;
         $display("x = %0d", x);
      end
   end

endmodule
