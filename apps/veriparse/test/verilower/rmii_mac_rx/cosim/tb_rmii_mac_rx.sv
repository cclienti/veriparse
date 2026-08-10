// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2025-2026 Christophe Clienti
//
// Three-way differential: the behavioural rewrite against BOTH its
// verilower output and the pre-existing production implementation
// (rmii_mac_rx_ref) — the reference is independent, not a transcription,
// so the bench checks the rewrite is faithful AND the lowering is exact.
// The frame driver replays the original testbench's scenarios
// procedurally: a clean frame, a carrier drop before the SFD (DROP), a
// backpressure pulse mid-payload (ERROR), backpressure covering the last
// beat (ERROR at LAST), and two near back-to-back frames.
//
// The behavioural process starts at time zero but idles in its wait
// states while the line is quiet through reset, so it aligns with the
// reference with no offset. The lowered machine leaves reset one cycle
// later: it reads every input through one-cycle delay registers and its
// outputs are compared against the behavioural outputs delayed by one —
// the usual convention. Divergence exits through $fatal.
module tb_rmii_mac_rx;
   logic clk = 0;
   logic srst = 1;
   logic [1:0] rxd = 2'b00;
   logic rxen = 0;
   logic tready = 1;
   logic [1:0] rxd_q = 2'b00;
   logic rxen_q = 0, tready_q = 1;

   logic tvalid_r, tlast_r, tuser_r;
   logic [1:0] tdata_r;
   logic tvalid_b, tlast_b, tuser_b;
   logic [1:0] tdata_b;
   logic tvalid_l, tlast_l, tuser_l;
   logic [1:0] tdata_l;

   logic tvalid_b_q = 0, tlast_b_q = 0, tuser_b_q = 0;
   logic [1:0] tdata_b_q = 0;

   int checked = 0;
   int tlast_pulses = 0;
   bit checking = 0;

   always #10 clk = ~clk;

   rmii_mac_rx_ref u_ref (.clock(clk), .srst(srst), .rxd(rxd), .rxen(rxen),
                          .axi_tvalid(tvalid_r), .axi_tlast(tlast_r), .axi_tdata(tdata_r),
                          .axi_tuser(tuser_r), .axi_tready(tready));
   rmii_mac_rx u_beh (.clock(clk), .srst(srst), .rxd(rxd), .rxen(rxen),
                      .axi_tvalid(tvalid_b), .axi_tlast(tlast_b), .axi_tdata(tdata_b),
                      .axi_tuser(tuser_b), .axi_tready(tready));
   rmii_mac_rx_lowered u_low (.clock(clk), .srst(srst), .rxd(rxd_q), .rxen(rxen_q),
                              .axi_tvalid(tvalid_l), .axi_tlast(tlast_l), .axi_tdata(tdata_l),
                              .axi_tuser(tuser_l), .axi_tready(tready_q));

   always @(posedge clk) begin
      rxd_q      <= rxd;
      rxen_q     <= rxen;
      tready_q   <= tready;
      tvalid_b_q <= tvalid_b;
      tlast_b_q  <= tlast_b;
      tuser_b_q  <= tuser_b;
      tdata_b_q  <= tdata_b;
   end

   // The rewrite against the production reference, same cycle.
   always @(posedge clk) if (checking) begin
      if (tvalid_b !== tvalid_r || tlast_b !== tlast_r || tuser_b !== tuser_r)
         $fatal(1, "DIVERGED beh/ref t=%0t  v=%b/%b l=%b/%b u=%b/%b",
                $time, tvalid_b, tvalid_r, tlast_b, tlast_r, tuser_b, tuser_r);
      if (tvalid_r && tdata_b !== tdata_r)
         $fatal(1, "DIVERGED beh/ref data t=%0t  %b/%b", $time, tdata_b, tdata_r);
      // The lowering against the rewrite, one cycle behind.
      if (tvalid_l !== tvalid_b_q || tlast_l !== tlast_b_q || tuser_l !== tuser_b_q)
         $fatal(1, "DIVERGED low/beh t=%0t  v=%b/%b l=%b/%b u=%b/%b",
                $time, tvalid_l, tvalid_b_q, tlast_l, tlast_b_q, tuser_l, tuser_b_q);
      if (tvalid_l && tdata_l !== tdata_b_q)
         $fatal(1, "DIVERGED low/beh data t=%0t  %b/%b", $time, tdata_l, tdata_b_q);
      checked++;
      if (tlast_r && tvalid_r) tlast_pulses++;
   end

   task automatic send_byte(input logic [7:0] value);
      for (int i = 0; i < 4; i++) begin
         rxd <= value[2*i +: 2];
         @(posedge clk);
      end
   endtask

   task automatic send_preamble_sfd(input int dibits);
      rxen <= 1'b1;
      repeat (dibits) begin
         rxd <= 2'b01;
         @(posedge clk);
      end
      send_byte(8'hD5);
   endtask

   task automatic send_frame(input logic [7:0] first, input int nbytes);
      send_preamble_sfd(12);
      for (int i = 0; i < nbytes; i++) send_byte(first + logic'(i[7:0]));
      rxen <= 1'b0;
      rxd  <= 2'b00;
      repeat (12) @(posedge clk);
   endtask

   initial begin
      repeat (3) @(posedge clk);
      srst <= 1'b0;
      checking <= 1'b1;
      repeat (6) @(posedge clk);

      // 1. a clean frame
      send_frame(8'h10, 6);

      // 2. carrier drop before the SFD: DROP, no beat
      rxen <= 1'b1;
      repeat (6) begin rxd <= 2'b01; @(posedge clk); end
      rxen <= 1'b0;
      rxd  <= 2'b00;
      repeat (12) @(posedge clk);

      // 3. backpressure mid-payload: ERROR
      fork
         send_frame(8'h30, 8);
         begin
            repeat (30) @(posedge clk);
            tready <= 1'b0;
            repeat (2) @(posedge clk);
            tready <= 1'b1;
         end
      join
      repeat (8) @(posedge clk);

      // 4. backpressure covering the last beat: ERROR at LAST
      fork
         send_frame(8'h50, 5);
         begin
            repeat (41) @(posedge clk);
            tready <= 1'b0;
            repeat (4) @(posedge clk);
            tready <= 1'b1;
         end
      join
      repeat (8) @(posedge clk);

      // 5. two frames nearly back to back
      send_frame(8'h70, 4);
      send_frame(8'h90, 4);

      repeat (20) @(posedge clk);
      $display("checked=%0d tlast_pulses=%0d", checked, tlast_pulses);
      if (tlast_pulses < 4)
         $fatal(1, "expected at least 4 frame terminations, saw %0d", tlast_pulses);
      $display("EQUIVALENT checked=%0d", checked);
      $finish;
   end
endmodule
