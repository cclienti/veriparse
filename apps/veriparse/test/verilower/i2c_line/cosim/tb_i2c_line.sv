// §7.4's differential proof: the Appendix-B byte write with the LOW/HIGH
// halves as one task called per half-bit — eighteen call sites' worth of
// states from one task text, the constant hold substituting, the shared
// depth-1 countdown reloading per phase. Registered outputs only, so the
// sampling is the plain count_line convention: inputs skewed one cycle
// into the lowered machine, reference outputs compared through one
// posedge stage, $fatal on divergence — the exit status is the verdict.
// Two back-to-back frames cover §A.3's frame-chaining case.
module tb_i2c_line;
   logic clk = 0, rst_n = 0;
   logic start = 0, start_q = 0;
   logic [7:0] byte_in = 8'h00, byte_in_q = 8'h00;
   logic scl_b, sda_b, busy_b, done_b;
   logic scl_l, sda_l, busy_l, done_l;
   logic scl_b_q = 1, sda_b_q = 1, busy_b_q = 0, done_b_q = 0;
   int checked = 0;

   always #5 clk = ~clk;

   i2c_line u_beh (.clk(clk), .rst_n(rst_n), .start(start), .byte_in(byte_in),
                   .scl(scl_b), .sda(sda_b), .busy(busy_b), .done(done_b));
   i2c_line_lowered u_low (.clk(clk), .rst_n(rst_n), .start(start_q),
                           .byte_in(byte_in_q), .scl(scl_l), .sda(sda_l),
                           .busy(busy_l), .done(done_l));

   always @(posedge clk) begin
      start_q   <= start;
      byte_in_q <= byte_in;
      scl_b_q   <= scl_b;
      sda_b_q   <= sda_b;
      busy_b_q  <= busy_b;
      done_b_q  <= done_b;
   end

   initial begin
      @(posedge clk);          // the lowered machine takes its reset here
      rst_n <= 1'b1;
      repeat (2) @(posedge clk);
      byte_in <= 8'hA5;
      for (int k = 0; k < 300; k++) begin
         @(posedge clk);
         if (scl_l !== scl_b_q || sda_l !== sda_b_q ||
             busy_l !== busy_b_q || done_l !== done_b_q)
            $fatal(1, "DIVERGED t=%0t  scl fsm=%b ref=%b  sda fsm=%b ref=%b  busy fsm=%b ref=%b  done fsm=%b ref=%b",
                   $time, scl_l, scl_b_q, sda_l, sda_b_q, busy_l, busy_b_q,
                   done_l, done_b_q);
         checked++;
         start <= (k % 90) < 2 || (k > 95 && k < 98);
         if (k == 95) byte_in <= 8'h3C;
      end
      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
