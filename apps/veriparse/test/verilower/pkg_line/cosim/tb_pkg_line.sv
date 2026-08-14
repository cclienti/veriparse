// The package-task differential proof: the multi-cycle hold lives in a
// package, so every signal it touches travels through its formals (IEEE
// §26.2) — the clock by const ref, the count by input, captured at entry
// (§12.7.3). Registered outputs only, so the sampling is the plain
// count_line convention: inputs skewed one cycle into the lowered machine,
// reference outputs compared through one posedge stage, $fatal on
// divergence — the exit status is the verdict. The step sweep includes
// zero, the skip path of the dynamic countdown.
module tb_pkg_line;
   logic clk = 0, rst_n = 0;
   logic start = 0, start_q = 0;
   logic [7:0] step = 8'd0, step_q = 8'd0;
   logic [7:0] q_b, q_l;
   logic busy_b, busy_l, done_b, done_l;
   logic [7:0] q_b_q = '0;
   logic busy_b_q = 0, done_b_q = 0;
   int checked = 0;

   always #5 clk = ~clk;

   pkg_line u_beh (.clk(clk), .rst_n(rst_n), .start(start), .step(step),
                   .q(q_b), .busy(busy_b), .done(done_b));
   pkg_line_lowered u_low (.clk(clk), .rst_n(rst_n), .start(start_q),
                           .step(step_q), .q(q_l), .busy(busy_l),
                           .done(done_l));

   always @(posedge clk) begin
      start_q  <= start;
      step_q   <= step;
      q_b_q    <= q_b;
      busy_b_q <= busy_b;
      done_b_q <= done_b;
   end

   initial begin
      @(posedge clk);          // the lowered machine takes its reset here
      rst_n <= 1'b1;
      repeat (2) @(posedge clk);
      step <= 8'd3;
      for (int k = 0; k < 300; k++) begin
         @(posedge clk);
         if (q_l !== q_b_q || busy_l !== busy_b_q || done_l !== done_b_q)
            $fatal(1, "DIVERGED t=%0t  q fsm=%h ref=%h  busy fsm=%b ref=%b  done fsm=%b ref=%b",
                   $time, q_l, q_b_q, busy_l, busy_b_q, done_l, done_b_q);
         checked++;
         start <= (k % 40) < 2;
         if (k == 78)  step <= 8'd0;   // the zero-skip frame
         if (k == 118) step <= 8'd7;
      end
      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
