// The ADR's own worked example, differentially: the behavioural A.1
// transmitter against its verilower output. Perpetual machine, so the
// stimulus runs the §11.2 edges frame after frame: a single isolated
// frame, the back-to-back case — send held across the frame boundary, the
// machine restarting on the stop bit's last edge with no idle cycle — and
// randomized send/data.
//
// The reference starts at time zero regardless of reset, so it leads the
// machine by the one-cycle hold: its outputs are compared through
// one-cycle delay registers, and the driven inputs are read by the lowered
// machine through one-cycle delay registers so both models see the same
// value at the same step. Divergence exits through $fatal: the exit
// status is the verdict.
module tb_rs232_tx;
   logic clk = 0, rst_n = 0;
   logic send = 0, send_q = 0;
   logic [7:0] data = 8'hA5, data_q = 8'hA5;
   logic tx_b, busy_b, tx_l, busy_l;
   logic tx_b_q = 1, busy_b_q = 0;
   int checked = 0, rnd = 0;

   always #5 clk = ~clk;

   rs232_tx #(.BAUD_DIV(4)) u_beh (.clk(clk), .rst_n(rst_n), .data(data), .send(send),
                                   .tx(tx_b), .busy(busy_b));
   rs232_tx_lowered u_low (.clk(clk), .rst_n(rst_n), .data(data_q), .send(send_q),
                           .tx(tx_l), .busy(busy_l));

   always @(posedge clk) begin
      send_q    <= send;
      data_q    <= data;
      tx_b_q    <= tx_b;
      busy_b_q  <= busy_b;
   end

   task automatic check;
      if (tx_l !== tx_b_q || busy_l !== busy_b_q)
         $fatal(1, "DIVERGED t=%0t  tx fsm=%b ref=%b  busy fsm=%b ref=%b",
                $time, tx_l, tx_b_q, busy_l, busy_b_q);
      checked++;
   endtask

   initial begin
      @(posedge clk);          // the lowered machine takes its reset here
      rst_n <= 1'b1;
      repeat (2) @(posedge clk);

      // 1. a single isolated frame
      send <= 1'b1;
      @(posedge clk); check();
      send <= 1'b0;
      repeat (60) begin @(posedge clk); check(); end

      // 2. back-to-back: send held across the frame boundary
      send <= 1'b1;
      repeat (140) begin @(posedge clk); check(); end
      send <= 1'b0;
      repeat (60) begin @(posedge clk); check(); end

      // 3. random stimulus
      for (int k = 0; k < 400; k++) begin
         @(posedge clk); check();
         rnd = $random;
         if (k % 7  == 0) send <= rnd[0];
         if (k % 13 == 0) data <= rnd[15:8];
      end
      repeat (60) begin @(posedge clk); check(); end

      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
