module parameter0
  #(parameter integer          WIDTH       = 8,
    parameter [WIDTH-1:0][3:0] RESET_VALUE = -1)
   (input wire                  reset,
    input wire                  clk,
    input wire [WIDTH-1:0][3:0] in0,
    output reg [WIDTH-1:0][3:0] out);

   always @(posedge clk) begin
      if (reset) begin
         out <= RESET_VALUE;
      end
      else begin
         out <= in0;
      end
   end

endmodule
