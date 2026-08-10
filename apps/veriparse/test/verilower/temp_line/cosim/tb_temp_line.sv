// §6.1 temporaries under live data: a 9-bit sum whose declared width holds
// the carry the surrounding 8-bit context would drop — the materialized
// wire's whole point (§11.6) — and a second temporary in the next segment,
// alternating forever. Random operands drive the carry corner both ways.
//
// The reference starts at time zero regardless of reset, so it leads the
// machine by the one-cycle hold: its outputs are compared through
// one-cycle delay registers, and the lowered machine reads the operands
// through one-cycle delay registers so both models see the same values at
// the same step. Divergence exits through $fatal.
module tb_temp_line;
   logic clk = 0, rst_n = 0;
   logic [7:0] a = '0, b = '0;
   logic [7:0] a_q = '0, b_q = '0;
   logic [7:0] q_b, q_l;
   logic carry_b, carry_l;
   logic [7:0] q_b_q = 0;
   logic carry_b_q = 0;
   int checked = 0, rnd = 0;

   always #5 clk = ~clk;

   temp_line u_beh (.clk(clk), .rst_n(rst_n), .a(a), .b(b), .q(q_b), .carry(carry_b));
   temp_line_lowered u_low (.clk(clk), .rst_n(rst_n), .a(a_q), .b(b_q),
                            .q(q_l), .carry(carry_l));

   always @(posedge clk) begin
      a_q       <= a;
      b_q       <= b;
      q_b_q     <= q_b;
      carry_b_q <= carry_b;
   end

   task automatic step;
      @(posedge clk);
      if (q_l !== q_b_q || carry_l !== carry_b_q)
         $fatal(1, "DIVERGED t=%0t  q fsm=%0d ref=%0d  carry fsm=%b ref=%b",
                $time, q_l, q_b_q, carry_l, carry_b_q);
      checked++;
   endtask

   initial begin
      rnd = $random;
      a   = rnd[7:0];
      b   = rnd[15:8];
      @(posedge clk);          // the lowered machine takes its reset here
      rst_n <= 1'b1;
      repeat (2) @(posedge clk);
      for (int k = 0; k < 50; k++) begin
         step();
         rnd = $random;
         a <= rnd[7:0];
         b <= rnd[15:8];
      end
      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
