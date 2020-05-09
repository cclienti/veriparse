module function4();

   function automatic integer clogb2;
      input value;
      reg [31:0] value;
      begin
         if (value > 1) begin
            clogb2 = clogb2(value >> 1) + 1;
         end
         else begin
            clogb2 = 1;
         end
      end
   endfunction

   initial begin
      $display(clogb2(53));
   end

endmodule
