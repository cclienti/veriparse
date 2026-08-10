// Loops make the back-edges observable (ADR-0014 §11.2): the §7.3
// wait-state's self-loop taken while start is low and exited when it
// rises, the constant countdown's lap taken five times — first and last
// iteration both committing a tick — the rolled for's induced index with
// its §6.1 forward substitution read per lap into sum, the captured
// countdown's entry guard both entering (n=3) and skipping (n=0), and the
// §8 jumps: continue while tick_en is low, the committing lap while it is
// high, break when start finally drops. Three behavioural/lowered pairs:
//   pair 0: start driven low then high then low,  n=3
//   pair 1: start driven low then high then low,  n=0 — the zero count skips
//   pair 2: start tied high,                      n=3 — the wait state never
//           entered, the jump loop never broken
//
// The reference starts at time zero regardless of reset, so it leads the
// machine by the one-cycle hold: its outputs are compared through
// one-cycle delay registers, and the driven inputs are read by the lowered
// pairs through one-cycle delay registers so both models see the same
// value at the same step. Divergence exits through $fatal: the exit
// status is the verdict.
module tb_count_line;
   logic clk = 0, rst_n = 0;
   logic start = 0, start_q = 0;
   logic tick_en = 0, tick_en_q = 0;
   logic busy_b0, busy_l0, busy_b1, busy_l1, busy_b2, busy_l2;
   logic done_b0, done_l0, done_b1, done_l1, done_b2, done_l2;
   logic [7:0] ticks_b0, ticks_l0, ticks_b1, ticks_l1, ticks_b2, ticks_l2;
   logic [7:0] sum_b0, sum_l0, sum_b1, sum_l1, sum_b2, sum_l2;
   logic busy_b0_q = 0, busy_b1_q = 0, busy_b2_q = 0;
   logic done_b0_q = 0, done_b1_q = 0, done_b2_q = 0;
   logic [7:0] ticks_b0_q = 0, ticks_b1_q = 0, ticks_b2_q = 0;
   logic [7:0] sum_b0_q = 0, sum_b1_q = 0, sum_b2_q = 0;
   int checked = 0;

   always #5 clk = ~clk;

   count_line u_beh0 (.clk(clk), .rst_n(rst_n), .start(start), .tick_en(tick_en), .n(4'd3),
                      .busy(busy_b0), .ticks(ticks_b0), .sum(sum_b0), .done(done_b0));
   count_line_lowered u_low0 (.clk(clk), .rst_n(rst_n), .start(start_q), .tick_en(tick_en_q),
                              .n(4'd3), .busy(busy_l0), .ticks(ticks_l0), .sum(sum_l0),
                              .done(done_l0));
   count_line u_beh1 (.clk(clk), .rst_n(rst_n), .start(start), .tick_en(tick_en), .n(4'd0),
                      .busy(busy_b1), .ticks(ticks_b1), .sum(sum_b1), .done(done_b1));
   count_line_lowered u_low1 (.clk(clk), .rst_n(rst_n), .start(start_q), .tick_en(tick_en_q),
                              .n(4'd0), .busy(busy_l1), .ticks(ticks_l1), .sum(sum_l1),
                              .done(done_l1));
   count_line u_beh2 (.clk(clk), .rst_n(rst_n), .start(1'b1), .tick_en(tick_en), .n(4'd3),
                      .busy(busy_b2), .ticks(ticks_b2), .sum(sum_b2), .done(done_b2));
   count_line_lowered u_low2 (.clk(clk), .rst_n(rst_n), .start(1'b1), .tick_en(tick_en_q),
                              .n(4'd3), .busy(busy_l2), .ticks(ticks_l2), .sum(sum_l2),
                              .done(done_l2));

   always @(posedge clk) begin
      start_q    <= start;
      tick_en_q  <= tick_en;
      busy_b0_q  <= busy_b0;
      done_b0_q  <= done_b0;
      ticks_b0_q <= ticks_b0;
      sum_b0_q   <= sum_b0;
      busy_b1_q  <= busy_b1;
      done_b1_q  <= done_b1;
      ticks_b1_q <= ticks_b1;
      sum_b1_q   <= sum_b1;
      busy_b2_q  <= busy_b2;
      done_b2_q  <= done_b2;
      ticks_b2_q <= ticks_b2;
      sum_b2_q   <= sum_b2;
   end

   initial begin
      @(posedge clk);          // the lowered machines take their reset here
      rst_n <= 1'b1;
      repeat (2) @(posedge clk);
      for (int k = 0; k < 60; k++) begin
         @(posedge clk);
         if(busy_l0 !== busy_b0_q || ticks_l0 !== ticks_b0_q || sum_l0 !== sum_b0_q ||
            done_l0 !== done_b0_q)
            $fatal(1, "DIVERGED pair0 t=%0t  busy fsm=%b ref=%b  ticks fsm=%0d ref=%0d  sum fsm=%0d ref=%0d  done fsm=%b ref=%b",
                   $time, busy_l0, busy_b0_q, ticks_l0, ticks_b0_q, sum_l0, sum_b0_q,
                   done_l0, done_b0_q);
         if(busy_l1 !== busy_b1_q || ticks_l1 !== ticks_b1_q || sum_l1 !== sum_b1_q ||
            done_l1 !== done_b1_q)
            $fatal(1, "DIVERGED pair1 t=%0t  busy fsm=%b ref=%b  ticks fsm=%0d ref=%0d  sum fsm=%0d ref=%0d  done fsm=%b ref=%b",
                   $time, busy_l1, busy_b1_q, ticks_l1, ticks_b1_q, sum_l1, sum_b1_q,
                   done_l1, done_b1_q);
         if(busy_l2 !== busy_b2_q || ticks_l2 !== ticks_b2_q || sum_l2 !== sum_b2_q ||
            done_l2 !== done_b2_q)
            $fatal(1, "DIVERGED pair2 t=%0t  busy fsm=%b ref=%b  ticks fsm=%0d ref=%0d  sum fsm=%0d ref=%0d  done fsm=%b ref=%b",
                   $time, busy_l2, busy_b2_q, ticks_l2, ticks_b2_q, sum_l2, sum_b2_q,
                   done_l2, done_b2_q);
         checked++;
         if (k == 5) start <= 1'b1;
         if (k == 40) start <= 1'b0;
         tick_en <= k[1];
      end
      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
