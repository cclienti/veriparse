`include "math.vh"
module top(
  input  wire [7:0] x,
  input  wire [7:0] y,
  output wire [7:0] hi,
  output wire [7:0] lo
);
  assign hi = `MAX(x, y);
  assign lo = `MIN(x, y);
endmodule
