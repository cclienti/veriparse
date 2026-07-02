module vf_partial_write0;

   reg [7:0] f;
   reg [7:0] y;

   initial begin
      f = 8'd5;
      f[0] = 1'b0;
      y = f;
   end

endmodule
