module instance4 (in0, in1, out0, out1, clk);
   input [2:0]  in0, in1;
   output [2:0] out0, out1;
   input        clk;

   wire [9:1]   t0;
   wire [21:1]  t1;

   dffn #(3) p0 [1:4]   ({out0, t0}, {t0, in0}, clk),
             p1 [1:8] ({out1, t1}, {t1, in1}, clk);
endmodule

module DFF (output reg q,
            input wire d, clk);
   always @(posedge clk) q <= d;
endmodule

module dffn (q, d, clk);
   input [2:0]  d;
   output [2:0] q;
   input        clk;

   DFF dff[2:0] (q, d, clk);
endmodule
