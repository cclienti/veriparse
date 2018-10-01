module localparam0
  #(parameter integer LOG_WIDTH = 3)
  (input wire             reset,
   input wire             clk,
   input wire [2**LOG_WIDTH:0] in,
   output reg [2**LOG_WIDTH:0] out);

   localparam integer WIDTH3 = WIDTH2+1;
   localparam integer WIDTH2 = WIDTH*2;
   localparam integer WIDTH = 2**LOG_WIDTH-1;

   always @(posedge clock, negedge reset) begin
      if (reset == 1'b0) begin
         out <= WIDTH3 + 1;
      end
      else begin
         out <= in;
      end
   end


endmodule
