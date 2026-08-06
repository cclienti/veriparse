module no_unroll0 (input clk, output reg [7:0] r);
   integer i;

   initial begin
      r = 0;

      (* veriparse_no_unroll *)
      repeat (4) @(posedge clk);

      (* veriparse_no_unroll *)
      for (i = 0; i < 2; i = i + 1) begin
         r = r + 1;
         repeat (2) r = r + 2;
      end

      repeat (2) r = r + 3;
   end
endmodule
