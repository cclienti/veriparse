module always4
  (input wire       reset,
   input wire       clk,
   input wire [7:0] in0, in1, in2, in3,
   output reg [7:0] out);

   always @(in0[3:0], in1[3:0], in2[7:4], in3[3:0]) begin
      out = {in0, in1} | {in2, in3};
   end

endmodule
