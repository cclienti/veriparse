module function3();

   function automatic integer clogb2(input reg [31:0] value, a, input b);
      begin
         if (value > 1) begin
            clogb2 = clogb2(value >> 1) + 1;
         end
         else begin
            clogb2 = 1;
         end
      end
   endfunction

endmodule
