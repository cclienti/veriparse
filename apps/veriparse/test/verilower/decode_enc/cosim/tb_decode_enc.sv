// §6.2 output encoding under the same cycle-sampled comparison as
// decode_line: the state bits ARE busy and done here, so this bench pins
// that the composed encoding, the disambiguation field, and the slice
// assigns reproduce the behavioural source exactly — the sampling
// conventions (input skew, two posedge stages on the blocking-assigned
// reference outputs, and the scheduling probe) are decode_line's,
// documented there.
module tb_decode_enc;
   logic clk = 0, rst_n = 0;
   logic start = 0, start_q = 0;
   logic [7:0] q_b, q_l;
   logic busy_b, busy_l, done_b, done_l;
   logic [7:0] q_b_q = '0;
   logic busy_bn = 0, done_bn = 0;
   logic busy_b_q = 0, done_b_q = 0;
   int checked = 0;

   always #5 clk = ~clk;

   decode_enc u_beh (.clk(clk), .rst_n(rst_n), .start(start),
                     .q(q_b), .busy(busy_b), .done(done_b));
   decode_enc_lowered u_low (.clk(clk), .rst_n(rst_n), .start(start_q),
                             .q(q_l), .busy(busy_l), .done(done_l));

   logic probe = 1'b0, probe_seen = 1'b0;
   initial forever begin
      @(posedge clk);
      probe = ~probe;
   end
   always @(posedge clk) probe_seen <= probe;
   initial begin
      repeat (3) @(posedge clk);
      @(negedge clk);
      if (probe_seen !== probe)
         $fatal(1, "BENCH ASSUMPTION BROKEN: the scheduler runs samplers before woken processes");
   end

   always @(posedge clk) begin
      start_q  <= start;
      q_b_q    <= q_b;
      busy_bn  <= busy_b;
      done_bn  <= done_b;
      busy_b_q <= busy_bn;
      done_b_q <= done_bn;
   end

   initial begin
      @(posedge clk);          // the lowered machine takes its reset here
      rst_n <= 1'b1;
      repeat (2) @(posedge clk);
      for (int k = 0; k < 90; k++) begin
         @(posedge clk);
         if (busy_l !== busy_b_q || done_l !== done_b_q || q_l !== q_b_q)
            $fatal(1, "DIVERGED t=%0t  busy fsm=%b ref=%b  done fsm=%b ref=%b  q fsm=%0d ref=%0d",
                   $time, busy_l, busy_b_q, done_l, done_b_q, q_l, q_b_q);
         checked++;
         start <= (k % 9) < 2;
      end
      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
