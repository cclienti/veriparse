// Branches make the path cover observable: an if/else with arms of unequal
// length, a cut-point-free conditional riding inside a segment, and a
// no-default case whose no-match path falls through. Both fork directions
// must run to cover the merge from every arm, so two behavioural/lowered
// pairs share the bench with `mode` pinned low and high.
//
// The reference starts at time zero regardless of reset, so it leads the
// machine by the one-cycle hold (ADR-0014 §11): its outputs are compared
// through one-cycle delay registers, and the lowered pair reads `d` through
// a one-cycle delay register so both models see the same value at the same
// step. Divergence exits through $fatal: the exit status is the verdict.
module tb_branch_line;
   logic clk = 0, rst_n = 0;
   logic [7:0] d = '0;
   logic [7:0] d_q = '0;
   logic done_b0, done_l0, done_b1, done_l1;
   logic [7:0] acc_b0, acc_l0, acc_b1, acc_l1;
   logic done_b0_q = 0, done_b1_q = 0;
   logic [7:0] acc_b0_q = 0, acc_b1_q = 0;
   int checked = 0, rnd = 0;

   always #5 clk = ~clk;

   branch_line u_beh0 (.clk(clk), .rst_n(rst_n), .mode(1'b0), .d(d),
                       .done(done_b0), .acc(acc_b0));
   branch_line_lowered u_low0 (.clk(clk), .rst_n(rst_n), .mode(1'b0), .d(d_q),
                               .done(done_l0), .acc(acc_l0));
   branch_line u_beh1 (.clk(clk), .rst_n(rst_n), .mode(1'b1), .d(d),
                       .done(done_b1), .acc(acc_b1));
   branch_line_lowered u_low1 (.clk(clk), .rst_n(rst_n), .mode(1'b1), .d(d_q),
                               .done(done_l1), .acc(acc_l1));

   always @(posedge clk) begin
      d_q       <= d;
      acc_b0_q  <= acc_b0;
      done_b0_q <= done_b0;
      acc_b1_q  <= acc_b1;
      done_b1_q <= done_b1;
   end

   initial begin
      @(posedge clk);          // the lowered machines take their reset here
      rst_n <= 1'b1;
      repeat (2) @(posedge clk);
      for (int k = 0; k < 30; k++) begin
         @(posedge clk);
         if (acc_l0 !== acc_b0_q || done_l0 !== done_b0_q)
            $fatal(1, "DIVERGED mode=0 t=%0t  acc fsm=%0d ref=%0d  done fsm=%b ref=%b",
                   $time, acc_l0, acc_b0_q, done_l0, done_b0_q);
         if (acc_l1 !== acc_b1_q || done_l1 !== done_b1_q)
            $fatal(1, "DIVERGED mode=1 t=%0t  acc fsm=%0d ref=%0d  done fsm=%b ref=%b",
                   $time, acc_l1, acc_b1_q, done_l1, done_b1_q);
         checked++;
         rnd = $random;
         d <= rnd[7:0];
      end
      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
