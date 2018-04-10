module function0();

   function integer clogb2(input [31:0] value);
      reg [31:0] tmp;
      begin
         tmp = value - 1;
         for (clogb2 = 0; tmp > 0; clogb2 = clogb2 + 1) begin
            tmp = tmp >> 1;
         end
      end
   endfunction

   reg [31:0] val;
   reg [31:0] clog_reg;

   initial begin
      val = 53;
      clog_reg = clogb2(val);
      $display("clogb2(%0d) = %0d", val, clog_reg);
   end

endmodule
