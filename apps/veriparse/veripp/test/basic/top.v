`include "widths.vh"
module top(
  input  wire clk,
  input  wire [`DATA_W-1:0] d,
  input  wire [`ADDR_W-1:0] a,
  output reg  [`DATA_W-1:0] q
);
  always @(posedge clk) begin
    q <= d & {`DATA_W{a[0]}};
  end
endmodule
