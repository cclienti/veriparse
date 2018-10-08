module generate2;
   localparam DEC = 1;
   localparam SHIFT = 1;

   generate
      function integer clogb2(input [31:0] value);
         integer SHIFT;
         reg [31:0] tmp;
         begin
            SHIFT = 1;
            tmp = value - DEC;
            for (clogb2 = 0; tmp > 0; clogb2 = clogb2 + 1) begin
               tmp = tmp >> SHIFT;
            end
         end
      endfunction
   endgenerate

   reg [31:0] val;
   reg [31:0] clog_reg;

   initial begin
      val = 53;
      clog_reg = clogb2(val);
      $display("clogb2(%0d) = %0d", val, clog_reg);
   end


endmodule
