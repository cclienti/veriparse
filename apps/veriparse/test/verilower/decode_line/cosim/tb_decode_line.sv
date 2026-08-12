// §6.2's tracking-vs-holding window is exactly what the cycle-sampled
// comparison probes: the behavioural source holds each decoded value from
// the edge that computed it, the lowered always_comb tracks the state
// register — sampled at the edges through one-cycle delay registers, the
// two must agree on busy/done and on the fork-arrival tree for sel, while
// q checks the registered half against the same laps. Inputs are skewed
// through _q registers as everywhere in this suite; divergence exits
// through $fatal.
//
// One §6.2-specific convention: the reference's decoded outputs are
// blocking-assigned in the active region of the very edge a posedge
// sampler would read them, so that sampler races and loses its cycle of
// delay. A race-free NEGEDGE stage followed by the ordinary posedge stage
// restores exactly the two effective delays the nonblocking-driven q gets
// for free (its sampler's commit plus the check's pre-update read).
module tb_decode_line;
   logic clk = 0, rst_n = 0;
   logic start = 0, start_q = 0;
   logic mode = 0, mode_q = 0;
   logic [7:0] q_b, q_l;
   logic busy_b, busy_l, done_b, done_l, sel_b, sel_l;
   logic [7:0] q_b_q = '0;
   logic busy_bn = 0, done_bn = 0, sel_bn = 0;
   logic busy_b_q = 0, done_b_q = 0, sel_b_q = 0;
   int checked = 0;

   always #5 clk = ~clk;

   decode_line u_beh (.clk(clk), .rst_n(rst_n), .start(start), .mode(mode),
                      .q(q_b), .busy(busy_b), .done(done_b), .sel(sel_b));
   decode_line_lowered u_low (.clk(clk), .rst_n(rst_n), .start(start_q), .mode(mode_q),
                              .q(q_l), .busy(busy_l), .done(done_l), .sel(sel_l));

   always @(posedge clk) begin
      start_q  <= start;
      mode_q   <= mode;
      q_b_q    <= q_b;
      busy_b_q <= busy_bn;
      done_b_q <= done_bn;
      sel_b_q  <= sel_bn;
   end

   always @(negedge clk) begin
      busy_bn <= busy_b;
      done_bn <= done_b;
      sel_bn  <= sel_b;
   end

   initial begin
      @(posedge clk);          // the lowered machine takes its reset here
      rst_n <= 1'b1;
      repeat (2) @(posedge clk);
      for (int k = 0; k < 100; k++) begin
         @(posedge clk);
         if (busy_l !== busy_b_q || done_l !== done_b_q || sel_l !== sel_b_q ||
             q_l !== q_b_q)
            $fatal(1, "DIVERGED t=%0t  busy fsm=%b ref=%b  done fsm=%b ref=%b  sel fsm=%b ref=%b  q fsm=%0d ref=%0d",
                   $time, busy_l, busy_b_q, done_l, done_b_q, sel_l, sel_b_q, q_l, q_b_q);
         checked++;
         start <= (k % 11) < 2;
         mode  <= k[2];
      end
      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
