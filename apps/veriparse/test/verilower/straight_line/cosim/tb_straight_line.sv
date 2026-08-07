// One bench, both models: the behavioural source is the golden model, and
// the verilower output — module renamed by --suffix — sits beside it. The
// reference starts at time zero regardless of reset, so it leads the
// machine by the one-cycle hold (ADR-0014 §11); the comparison reads it
// through one-cycle delay registers instead of shifting indices. All on
// posedge, comparator in the active region reading previous-edge NBA
// values, same convention for both models. Divergence exits through
// $fatal: the exit status is the verdict.
module tb_straight_line;
   logic clk = 0, rst_n = 0;
   logic done_b, done_l;
   logic [7:0] acc_b, acc_l;
   logic done_b_q = 0;
   logic [7:0] acc_b_q = 0;
   int checked = 0;

   always #5 clk = ~clk;

   straight_line u_beh (.clk(clk), .rst_n(rst_n), .done(done_b), .acc(acc_b));
   straight_line_lowered u_low (.clk(clk), .rst_n(rst_n), .done(done_l), .acc(acc_l));

   always @(posedge clk) begin
      acc_b_q  <= acc_b;
      done_b_q <= done_b;
   end

   initial begin
      @(posedge clk);          // the lowered machine takes its reset here
      rst_n <= 1'b1;
      repeat (2) @(posedge clk);
      repeat (10) begin
         @(posedge clk);
         if (acc_l !== acc_b_q || done_l !== done_b_q)
            $fatal(1, "DIVERGED t=%0t  acc fsm=%0d ref=%0d  done fsm=%b ref=%b",
                   $time, acc_l, acc_b_q, done_l, done_b_q);
         checked++;
      end
      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
