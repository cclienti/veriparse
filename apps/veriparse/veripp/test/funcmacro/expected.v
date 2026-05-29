`line 1 "top.v" 0
`line 1 "math.vh" 1
`line 2 "top.v" 2
module top(
  input  wire [7:0] x,
  input  wire [7:0] y,
  output wire [7:0] hi,
  output wire [7:0] lo
);
  assign hi = ((x) > (y) ? (x) : (y));
  assign lo = ((x) < (y) ? (x) : (y));
endmodule
