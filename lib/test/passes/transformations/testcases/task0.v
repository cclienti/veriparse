module task0;

   localparam DEC = 1;
   localparam SHIFT = 1;

   task clogb2(input [31:0] value, output reg [31:0] clog);
      integer SHIFT;
      begin
         SHIFT = 1;
         value = value - DEC;
         for (clog = 0; value > 0; clog = clog + 1) begin
            value = value >> SHIFT;
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
