module constant_folding0
  #(parameter width = 2**2 * 2**3)
   (input wire clock,
    output reg [32-1:0] out1,
    output reg [2*32-1:0] out2,
    output reg [48-1:0] out3);

   always @(*) begin
      out1 = 1 + 2 * 12 - 3**5 + -1 + ((0 == 1) ? -1*(45/5) : 1);
   end

   always @(posedge clock) begin
      if (out1[32-31] == 1'b1) begin
         out2 <= {{33-1{1'b0}}, out1};
      end
   end

   always @(*) out3 = 48'b1111111100000000_0000000000000000_0000000000000000;

endmodule
