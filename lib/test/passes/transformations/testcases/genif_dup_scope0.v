// Two distinct conditional generate constructs may not share a block name
// (IEEE 1800-2017 §27.5: only the arms *within one* construct may, since at
// most one arm is instantiated). Here both conditions are true, so both
// blocks are instantiated and the name collides -- a hard error.
module genif_dup_scope0 (c, q);
   input c;
   output q;

   generate
      if (1) begin: g
         wire w;
         assign w = c;
      end
   endgenerate

   generate
      if (1) begin: g
         wire w;
         assign w = ~c;
      end
   endgenerate

   assign q = c;
endmodule
