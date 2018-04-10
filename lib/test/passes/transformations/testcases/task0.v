module task0;

   task clogb2(input [31:0] value, output reg [31:0] clog);
      begin
         value = value - 1;
         for (clog = 0; value > 0; clog = clog + 1) begin
            value = value >> 1;
         end
      end
   endtask

   reg [31:0] val;
   reg [31:0] clog_reg;

   initial begin
      val = 53;
      clogb2(val, clog_reg);
      $display("clogb2(%0d) = %0d", val, clog_reg);
   end

endmodule
