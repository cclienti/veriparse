// The enable half of the §5.3 emission is differentially checkable: the
// reference honours the enable through `iff`, so one bench drives both
// models with `en` toggling irregularly and compares every cycle. Inputs
// stay idle while reset is held (en low), so the reference's state is
// invariant over the hold and raw samples align with no offset
// (ADR-0014 §11). Divergence exits through $fatal.
module tb_enable_line;
   logic clk = 0, rst_n = 0, en = 0;
   logic done_b, done_l;
   logic [7:0] acc_b, acc_l;
   int checked = 0, rnd = 0;

   always #5 clk = ~clk;

   enable_line u_beh (.clk(clk), .rst_n(rst_n), .en(en),
                      .done(done_b), .acc(acc_b));
   enable_line_lowered u_low (.clk(clk), .rst_n(rst_n), .en(en),
                              .done(done_l), .acc(acc_l));

   initial begin
      repeat (3) @(posedge clk);
      rst_n <= 1'b1;
      for (int k = 0; k < 300; k++) begin
         @(posedge clk);
         if (acc_l !== acc_b || done_l !== done_b)
            $fatal(1, "DIVERGED t=%0t  acc fsm=%0d ref=%0d  done fsm=%b ref=%b",
                   $time, acc_l, acc_b, done_l, done_b);
         checked++;
         rnd = $random;
         if (k % 3 == 0) en <= rnd[0];
      end
      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
