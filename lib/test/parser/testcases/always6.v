module always6
  (input wire [1:0] reset,
   input wire [1:0] clk,
   input wire [7:0] in0, in1, in2, in3,
   output reg [7:0] out);

   always @(negedge reset[0] or posedge clk[1]) begin
      out <= in0 | in1 | in2 | in3;
   end

endmodule
