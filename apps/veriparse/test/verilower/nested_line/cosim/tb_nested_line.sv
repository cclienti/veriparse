// Nested rolled repeats: the 2D-scan shape — an outer counting timer whose
// body holds another. Each nesting depth owns its countdown register, so
// the inner reload no longer touches the outer's remaining count; the
// captured inner count also exercises the depth-1 entry guard with n=0
// (the inner loop skipped on every outer lap) beside n=2. Two
// behavioural/lowered pairs, inputs constant per pair.
//
// The reference starts at time zero regardless of reset, so it leads the
// machine by the one-cycle hold: its outputs are compared through
// one-cycle delay registers. Divergence exits through $fatal: the exit
// status is the verdict.
module tb_nested_line;
   logic clk = 0, rst_n = 0;
   logic [7:0] rows_b0, rows_l0, cols_b0, cols_l0;
   logic [7:0] rows_b1, rows_l1, cols_b1, cols_l1;
   logic done_b0, done_l0, done_b1, done_l1;
   logic [7:0] rows_b0_q = 0, cols_b0_q = 0, rows_b1_q = 0, cols_b1_q = 0;
   logic done_b0_q = 0, done_b1_q = 0;
   int checked = 0;

   always #5 clk = ~clk;

   nested_line u_beh0 (.clk(clk), .rst_n(rst_n), .n(4'd2),
                       .rows(rows_b0), .cols(cols_b0), .done(done_b0));
   nested_line_lowered u_low0 (.clk(clk), .rst_n(rst_n), .n(4'd2),
                               .rows(rows_l0), .cols(cols_l0), .done(done_l0));
   nested_line u_beh1 (.clk(clk), .rst_n(rst_n), .n(4'd0),
                       .rows(rows_b1), .cols(cols_b1), .done(done_b1));
   nested_line_lowered u_low1 (.clk(clk), .rst_n(rst_n), .n(4'd0),
                               .rows(rows_l1), .cols(cols_l1), .done(done_l1));

   always @(posedge clk) begin
      rows_b0_q <= rows_b0;
      cols_b0_q <= cols_b0;
      done_b0_q <= done_b0;
      rows_b1_q <= rows_b1;
      cols_b1_q <= cols_b1;
      done_b1_q <= done_b1;
   end

   initial begin
      @(posedge clk);          // the lowered machines take their reset here
      rst_n <= 1'b1;
      repeat (2) @(posedge clk);
      for (int k = 0; k < 40; k++) begin
         @(posedge clk);
         if(rows_l0 !== rows_b0_q || cols_l0 !== cols_b0_q || done_l0 !== done_b0_q)
            $fatal(1, "DIVERGED pair0 t=%0t  rows fsm=%0d ref=%0d  cols fsm=%0d ref=%0d  done fsm=%b ref=%b",
                   $time, rows_l0, rows_b0_q, cols_l0, cols_b0_q, done_l0, done_b0_q);
         if(rows_l1 !== rows_b1_q || cols_l1 !== cols_b1_q || done_l1 !== done_b1_q)
            $fatal(1, "DIVERGED pair1 t=%0t  rows fsm=%0d ref=%0d  cols fsm=%0d ref=%0d  done fsm=%b ref=%b",
                   $time, rows_l1, rows_b1_q, cols_l1, cols_b1_q, done_l1, done_b1_q);
         checked++;
      end
      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
