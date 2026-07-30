// A hierarchical reference into this module's own named loop scope with an
// out-of-range index names a scope that does not exist once the loop is
// unrolled -- it must be diagnosed, not emitted verbatim.
module loop_dangling_ref0 (c, q);
   input c;
   output q;

   genvar i;
   generate
      for (i = 0; i < 4; i = i + 1) begin: blk
         wire r;
         assign r = c;
      end
   endgenerate

   assign q = blk[9].r;
endmodule
