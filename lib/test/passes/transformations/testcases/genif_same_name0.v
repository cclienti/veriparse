// Both arms of a conditional generate may share their block name (IEEE
// 1800-2017 27.5 / 1364-2005 12.4.2): at most one arm is instantiated.
// The resolution pipeline walks both arms before branch selection, so
// the shared name must not be treated as a scope collision.

module genif_same_name0_sub (clk, sel, a, b, q);
   parameter OUTREG = 0;
   input clk;
   input sel;
   input [3:0] a;
   input [3:0] b;
   output [3:0] q;

   generate
      if (OUTREG != 0) begin: gen_sel
         reg [3:0] q_r;
         always @(posedge clk) begin
            q_r <= sel ? a : b;
         end
         assign q = q_r;
      end
      else begin: gen_sel
         assign q = sel ? a : b;
      end
   endgenerate
endmodule

module genif_same_name0 (clk, sel, a, b, q0, q1);
   input clk;
   input sel;
   input [3:0] a;
   input [3:0] b;
   output [3:0] q0;
   output [3:0] q1;

   genif_same_name0_sub #(.OUTREG(0)) u0 (.clk(clk), .sel(sel), .a(a), .b(b), .q(q0));
   genif_same_name0_sub #(.OUTREG(1)) u1 (.clk(clk), .sel(sel), .a(a), .b(b), .q(q1));
endmodule
