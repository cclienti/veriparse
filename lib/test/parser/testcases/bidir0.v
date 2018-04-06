module bidirec (oe, clk, inp, outp, bidir);
   input   oe;
   input   clk;
   input   [7:0] inp;
   output  [7:0] outp;
   inout   [7:0] bidir;

   reg     [7:0] a;
   reg     [7:0] b;

   assign bidir = oe ? a : 8'bZ ;
   assign outp  = b;

   always @ (posedge clk) begin
      b <= bidir;
      a <= inp;
   end

endmodule
