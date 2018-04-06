module always1
  (input wire       reset,
   input wire       clk,
   input wire [7:0] in0, in1, in2, in3,
   output reg [7:0] out);

   always @(*) begin
      out = in0 | in1 | in2 | in3;
   end

endmodule
