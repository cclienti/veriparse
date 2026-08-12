// The in-flattener slot (ADR-0014 §10.3, veriflat --fsm): one marked
// module instantiated twice with different parameters — the shape a
// per-module verilower run cannot compile (§15) — flattened with each
// instance's machine compiled at its own N. A fast (N=3) and a slow
// (N=6) pulse counter run from one irregular start train, so their busy
// windows and done pulses interleave differently and any cross-instance
// mixup diverges. Equivalence as elsewhere: the reference leads by the
// reset hold, inputs and outputs aligned through one-cycle delay
// registers, $fatal on divergence — the exit status is the verdict.
module tb_fsm_flat;
   logic clk = 0, rst_n = 0;
   logic start = 0, start_q = 0;
   logic [1:0] busy_b, busy_l, done_b, done_l;
   logic [1:0] busy_b_q = '0, done_b_q = '0;
   int checked = 0;

   always #5 clk = ~clk;

   fsm_flat u_beh (.clk(clk), .rst_n(rst_n), .start(start),
                   .busy(busy_b), .done(done_b));
   fsm_flat_lowered u_low (.clk(clk), .rst_n(rst_n), .start(start_q),
                           .busy(busy_l), .done(done_l));

   always @(posedge clk) begin
      start_q  <= start;
      busy_b_q <= busy_b;
      done_b_q <= done_b;
   end

   initial begin
      @(posedge clk);          // the lowered machines take their reset here
      rst_n <= 1'b1;
      repeat (2) @(posedge clk);
      for (int k = 0; k < 80; k++) begin
         @(posedge clk);
         if (busy_l !== busy_b_q || done_l !== done_b_q)
            $fatal(1, "DIVERGED t=%0t  busy fsm=%b ref=%b  done fsm=%b ref=%b",
                   $time, busy_l, busy_b_q, done_l, done_b_q);
         checked++;
         start <= (k % 13) < 2;
      end
      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
