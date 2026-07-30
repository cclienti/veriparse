// Both arms of one conditional generate may share a block name (IEEE
// 1800-2017 §27.5 / 1364-2005 §12.4.2) and may hold arbitrarily different
// content -- here inner named loops driven by differently-named genvars, so
// the two arms derive different rename suffixes for the same scope path.
module genif_arms_genvar0_sub (c, q);
   parameter SEL = 0;
   input c;
   output [1:0] q;

   genvar i, k;
   generate
      if (SEL != 0) begin: gen_sel
         for (i = 0; i < 2; i = i + 1) begin: b
            assign q[i] = c;
         end
      end
      else begin: gen_sel
         for (k = 0; k < 2; k = k + 1) begin: b
            assign q[k] = ~c;
         end
      end
   endgenerate
endmodule

module genif_arms_genvar0 (c, q0, q1);
   input c;
   output [1:0] q0;
   output [1:0] q1;

   genif_arms_genvar0_sub #(.SEL(0)) u0 (.c(c), .q(q0));
   genif_arms_genvar0_sub #(.SEL(1)) u1 (.c(c), .q(q1));
endmodule
