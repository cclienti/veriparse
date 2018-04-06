module if2
  (input wire       reset,
   input wire       clk,
   input wire [1:0] sel,
   input wire [7:0] in0, in1, in2, in3,
   output reg [7:0] out);


   always @(posedge clk) begin
      out <= (sel == 0) ? in0 :
             (sel == 1) ? in1 :
             (sel == 2) ? in2 + 1 : in3;
   end

endmodule
