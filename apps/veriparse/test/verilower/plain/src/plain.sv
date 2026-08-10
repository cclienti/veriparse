module plain (input logic clk, input logic rst_n, input logic d, output logic q);

   always_ff @(posedge clk) q <= d;

endmodule
