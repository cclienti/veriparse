// Mixed rolled nesting under jumps: a wait-state, then a counting repeat
// whose body holds a rolled for whose body holds another counting repeat
// with a captured count and a break — the countdown depths threading
// through the counterless for layer, the abandoned lap's commit coalesced
// under the next reload. Three behavioural/lowered pairs:
//   pair 0: n=2, kill low        — the dense scan
//   pair 1: n=0, kill low        — the inner timer skipped everywhere
//   pair 2: n=3, kill toggling   — break abandoning inner laps
//
// The reference starts at time zero regardless of reset, so it leads the
// machine by the one-cycle hold: its outputs are compared through
// one-cycle delay registers, and the driven inputs reach the lowered
// machines through one-cycle delay registers so both models see the same
// value at the same step. Divergence exits through $fatal: the exit
// status is the verdict.
module tb_mixed_line;
   logic clk = 0, rst_n = 0;
   logic start = 0, start_q = 0;
   logic kill = 0, kill_q = 0;
   logic [7:0] q_b0, q_l0, q_b1, q_l1, q_b2, q_l2;
   logic done_b0, done_l0, done_b1, done_l1, done_b2, done_l2;
   logic [7:0] q_b0_q = 0, q_b1_q = 0, q_b2_q = 0;
   logic done_b0_q = 0, done_b1_q = 0, done_b2_q = 0;
   int checked = 0;

   always #5 clk = ~clk;

   mixed_line u_beh0 (.clk(clk), .rst_n(rst_n), .start(start), .kill(1'b0), .n(4'd2),
                      .q(q_b0), .done(done_b0));
   mixed_line_lowered u_low0 (.clk(clk), .rst_n(rst_n), .start(start_q), .kill(1'b0), .n(4'd2),
                              .q(q_l0), .done(done_l0));
   mixed_line u_beh1 (.clk(clk), .rst_n(rst_n), .start(start), .kill(1'b0), .n(4'd0),
                      .q(q_b1), .done(done_b1));
   mixed_line_lowered u_low1 (.clk(clk), .rst_n(rst_n), .start(start_q), .kill(1'b0), .n(4'd0),
                              .q(q_l1), .done(done_l1));
   mixed_line u_beh2 (.clk(clk), .rst_n(rst_n), .start(start), .kill(kill), .n(4'd3),
                      .q(q_b2), .done(done_b2));
   mixed_line_lowered u_low2 (.clk(clk), .rst_n(rst_n), .start(start_q), .kill(kill_q), .n(4'd3),
                              .q(q_l2), .done(done_l2));

   always @(posedge clk) begin
      start_q  <= start;
      kill_q   <= kill;
      q_b0_q   <= q_b0;
      done_b0_q <= done_b0;
      q_b1_q   <= q_b1;
      done_b1_q <= done_b1;
      q_b2_q   <= q_b2;
      done_b2_q <= done_b2;
   end

   task automatic step;
      @(posedge clk);
      if (q_l0 !== q_b0_q || done_l0 !== done_b0_q)
         $fatal(1, "DIVERGED pair0 t=%0t  q fsm=%0d ref=%0d  done fsm=%b ref=%b",
                $time, q_l0, q_b0_q, done_l0, done_b0_q);
      if (q_l1 !== q_b1_q || done_l1 !== done_b1_q)
         $fatal(1, "DIVERGED pair1 t=%0t  q fsm=%0d ref=%0d  done fsm=%b ref=%b",
                $time, q_l1, q_b1_q, done_l1, done_b1_q);
      if (q_l2 !== q_b2_q || done_l2 !== done_b2_q)
         $fatal(1, "DIVERGED pair2 t=%0t  q fsm=%0d ref=%0d  done fsm=%b ref=%b",
                $time, q_l2, q_b2_q, done_l2, done_b2_q);
      checked++;
   endtask

   initial begin
      @(posedge clk);          // the lowered machines take their reset here
      rst_n <= 1'b1;
      repeat (2) step();
      repeat (4) step();
      start <= 1'b1;
      for (int k = 0; k < 40; k++) begin
         step();
         kill <= (k % 5 == 3);
      end
      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
