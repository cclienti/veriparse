module if1
  (input wire       reset,
   input wire       clk,
   input wire [1:0] sel,
   input wire [7:0] in0, in1, in2, in3,
   output reg [7:0] out);


   always @(posedge clk) begin
      if (sel == 0) begin
        out <= in0;
      end
      else if (sel == 1) begin
        out <= in1;
      end
      else if (sel == 2) begin
        out <= in2;
      end
      else begin
        out <= in3;
      end
   end

endmodule
