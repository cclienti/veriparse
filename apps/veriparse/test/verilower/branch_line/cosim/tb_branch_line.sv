// Branches make the path cover observable: an if/else with arms of unequal
// length, a cut-point-free conditional riding inside a segment, and a
// no-default case whose no-match path falls through. The machines are
// one-shot, so each behavioural/lowered pair walks exactly one path: five
// pairs pin the inputs so every edge runs — the else arm of the fork
// (pair 0), and with mode high the taken/untaken directions of the inner
// `if (acc[0])` and all three case outcomes, steered by the d constants:
//   d=0 -> acc 1, if taken,   acc 8'h57 -> case fall-through
//   d=2 -> acc 3, if taken,   acc 8'h55 -> case item 2'd1
//   d=3 -> acc 4, if untaken, acc 8'h04 -> case item 2'd0
//   d=1 -> acc 2, if untaken, acc 8'h02 -> case item 2'd2
//
// The reference starts at time zero regardless of reset, so it leads the
// machine by the one-cycle hold (ADR-0014 §11): its outputs are compared
// through one-cycle delay registers, and pair 0 — the one driven with
// changing data, randomized from before reset release so the sampled
// values are live — reads `d` through a one-cycle delay register so both
// models see the same value at the same step. Divergence exits through
// $fatal: the exit status is the verdict.
module tb_branch_line;
   logic clk = 0, rst_n = 0;
   logic [7:0] d = '0;
   logic [7:0] d_q = '0;
   logic done_b0, done_l0, done_b1, done_l1, done_b2, done_l2;
   logic done_b3, done_l3, done_b4, done_l4;
   logic [7:0] acc_b0, acc_l0, acc_b1, acc_l1, acc_b2, acc_l2;
   logic [7:0] acc_b3, acc_l3, acc_b4, acc_l4;
   logic done_b0_q = 0, done_b1_q = 0, done_b2_q = 0, done_b3_q = 0, done_b4_q = 0;
   logic [7:0] acc_b0_q = 0, acc_b1_q = 0, acc_b2_q = 0, acc_b3_q = 0, acc_b4_q = 0;
   int checked = 0, rnd = 0;

   always #5 clk = ~clk;

   branch_line u_beh0 (.clk(clk), .rst_n(rst_n), .mode(1'b0), .d(d),
                       .done(done_b0), .acc(acc_b0));
   branch_line_lowered u_low0 (.clk(clk), .rst_n(rst_n), .mode(1'b0), .d(d_q),
                               .done(done_l0), .acc(acc_l0));
   branch_line u_beh1 (.clk(clk), .rst_n(rst_n), .mode(1'b1), .d(8'd0),
                       .done(done_b1), .acc(acc_b1));
   branch_line_lowered u_low1 (.clk(clk), .rst_n(rst_n), .mode(1'b1), .d(8'd0),
                               .done(done_l1), .acc(acc_l1));
   branch_line u_beh2 (.clk(clk), .rst_n(rst_n), .mode(1'b1), .d(8'd2),
                       .done(done_b2), .acc(acc_b2));
   branch_line_lowered u_low2 (.clk(clk), .rst_n(rst_n), .mode(1'b1), .d(8'd2),
                               .done(done_l2), .acc(acc_l2));
   branch_line u_beh3 (.clk(clk), .rst_n(rst_n), .mode(1'b1), .d(8'd3),
                       .done(done_b3), .acc(acc_b3));
   branch_line_lowered u_low3 (.clk(clk), .rst_n(rst_n), .mode(1'b1), .d(8'd3),
                               .done(done_l3), .acc(acc_l3));
   branch_line u_beh4 (.clk(clk), .rst_n(rst_n), .mode(1'b1), .d(8'd1),
                       .done(done_b4), .acc(acc_b4));
   branch_line_lowered u_low4 (.clk(clk), .rst_n(rst_n), .mode(1'b1), .d(8'd1),
                               .done(done_l4), .acc(acc_l4));

   always @(posedge clk) begin
      d_q       <= d;
      acc_b0_q  <= acc_b0;
      done_b0_q <= done_b0;
      acc_b1_q  <= acc_b1;
      done_b1_q <= done_b1;
      acc_b2_q  <= acc_b2;
      done_b2_q <= done_b2;
      acc_b3_q  <= acc_b3;
      done_b3_q <= done_b3;
      acc_b4_q  <= acc_b4;
      done_b4_q <= done_b4;
   end

   initial begin
      rnd = $random;
      d   = rnd[7:0];
      @(posedge clk);          // the lowered machines take their reset here
      rst_n <= 1'b1;
      repeat (2) @(posedge clk);
      for (int k = 0; k < 30; k++) begin
         @(posedge clk);
         if (acc_l0 !== acc_b0_q || done_l0 !== done_b0_q)
            $fatal(1, "DIVERGED pair0 t=%0t  acc fsm=%0d ref=%0d  done fsm=%b ref=%b",
                   $time, acc_l0, acc_b0_q, done_l0, done_b0_q);
         if (acc_l1 !== acc_b1_q || done_l1 !== done_b1_q)
            $fatal(1, "DIVERGED pair1 t=%0t  acc fsm=%0d ref=%0d  done fsm=%b ref=%b",
                   $time, acc_l1, acc_b1_q, done_l1, done_b1_q);
         if (acc_l2 !== acc_b2_q || done_l2 !== done_b2_q)
            $fatal(1, "DIVERGED pair2 t=%0t  acc fsm=%0d ref=%0d  done fsm=%b ref=%b",
                   $time, acc_l2, acc_b2_q, done_l2, done_b2_q);
         if (acc_l3 !== acc_b3_q || done_l3 !== done_b3_q)
            $fatal(1, "DIVERGED pair3 t=%0t  acc fsm=%0d ref=%0d  done fsm=%b ref=%b",
                   $time, acc_l3, acc_b3_q, done_l3, done_b3_q);
         if (acc_l4 !== acc_b4_q || done_l4 !== done_b4_q)
            $fatal(1, "DIVERGED pair4 t=%0t  acc fsm=%0d ref=%0d  done fsm=%b ref=%b",
                   $time, acc_l4, acc_b4_q, done_l4, done_b4_q);
         checked++;
         rnd = $random;
         d <= rnd[7:0];
      end
      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
