module if0
  (input wire       reset,
   input wire       clk,
   input wire [1:0] sel,
   input wire [7:0] in0, in1, in2, in3,
   output reg [7:0] out);


   always @(posedge clk) begin
      if (sel == 0)
        out <= in0;
      else if (sel == 1)
        out <= in1;
      else if (sel == 2)
        out <= in2;
      else
        out <= in3;
   end

endmodule
