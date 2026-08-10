// The receiver differentially: behavioural source against its verilower
// output, with the line driven through good frames, a glitch shorter than
// the half-baud alignment — the `continue` path back to the hunt — and a
// frame whose stop bit is low, the framing-error `continue`. The last
// frame checks the machine recovers and still receives.
//
// The reference starts at time zero regardless of reset, so it leads the
// machine by the one-cycle hold: its outputs are compared through
// one-cycle delay registers, and the lowered machine reads the line
// through a one-cycle delay register so both models see the same value at
// the same step. Divergence exits through $fatal: the exit status is the
// verdict.
module tb_rs232_rx;
   localparam int BAUD_DIV = 4;

   logic clk = 0, rst_n = 0;
   logic rx = 1, rx_q = 1;
   logic [7:0] data_b, data_l;
   logic valid_b, valid_l;
   logic [7:0] data_b_q = 0;
   logic valid_b_q = 0;
   int checked = 0;

   always #5 clk = ~clk;

   rs232_rx #(.BAUD_DIV(BAUD_DIV)) u_beh (.clk(clk), .rst_n(rst_n), .rx(rx),
                                          .data(data_b), .valid(valid_b));
   rs232_rx_lowered u_low (.clk(clk), .rst_n(rst_n), .rx(rx_q),
                           .data(data_l), .valid(valid_l));

   always @(posedge clk) begin
      rx_q      <= rx;
      data_b_q  <= data_b;
      valid_b_q <= valid_b;
   end

   task automatic step;
      @(posedge clk);
      if (data_l !== data_b_q || valid_l !== valid_b_q)
         $fatal(1, "DIVERGED t=%0t  data fsm=%02h ref=%02h  valid fsm=%b ref=%b",
                $time, data_l, data_b_q, valid_l, valid_b_q);
      checked++;
   endtask

   task automatic hold_line(input logic level, input int cycles);
      rx <= level;
      repeat (cycles) step();
   endtask

   task automatic send_frame(input logic [7:0] byte_val, input logic stop_level);
      hold_line(1'b0, BAUD_DIV);                     // start bit
      for (int b = 0; b < 8; b++)
         hold_line(byte_val[b], BAUD_DIV);           // LSB first
      hold_line(stop_level, BAUD_DIV);               // stop bit
      hold_line(1'b1, 2 * BAUD_DIV);                 // idle gap
   endtask

   initial begin
      @(posedge clk);          // the lowered machine takes its reset here
      rst_n <= 1'b1;
      repeat (2) step();
      hold_line(1'b1, 4);                            // idle line

      send_frame(8'hA5, 1'b1);                       // a good frame
      hold_line(1'b0, 1);                            // a one-cycle glitch:
      hold_line(1'b1, 3 * BAUD_DIV);                 // shorter than ALIGN
      send_frame(8'h3C, 1'b0);                       // framing error: dropped
      send_frame(8'h5A, 1'b1);                       // recovery frame
      hold_line(1'b1, 4 * BAUD_DIV);                 // drain

      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
