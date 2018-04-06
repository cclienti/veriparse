`default_nettype integer

module MxN_pipeline (in0, in1, out0, out1, clk);
   parameter M=3, N=4;

   input [M-1:0]    in0, in1;
   output [M-1:0]   out0, out1;
   input            clk;

   wire [M*(N-1):1] t0;
   wire [M*(2*N-1):1] t1;

   // It must produce a shift register of M-bits with a depth of N
   // elements.
   // #(M) redefines the bits parameter for dffn
   // create p[1:N] columns of dffn rows (pipeline)

   dffn #(M) p0 [1:N]   ({out0, t0}, {t0, in0}, clk),
             p1 [1:2*N] ({out1, t1}, {t1, in1}, clk);
endmodule
