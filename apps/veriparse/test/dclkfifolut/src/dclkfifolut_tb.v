//                              -*- Mode: Verilog -*-
// Filename        : dclkfifolut_tb.v
// Description     : Testbench of Dual Clock Fifo based on LUT memory
// Author          : Christophe Clienti
// Created On      : Tue Jun 18 14:35:09 2013
// Last Modified By: Christophe Clienti
// Last Modified On: Tue Jun 18 14:35:09 2013
// Update Count    : 0
// Status          : Unknown, Use with caution!
// Copyright (C) 2013-2016 Christophe Clienti - All Rights Reserved

`timescale 1 ns / 100 ps


module dclkfifolut_tb;

   //----------------------------------------------------------------
   //Constants
   //----------------------------------------------------------------
   localparam LOG2_FIFO_DEPTH = 3;
   localparam FIFO_WIDTH      = 8;

   //----------------------------------------------------------------
   //Signals
   //----------------------------------------------------------------
   //DUT Signals
   reg                      rsrst;
   reg                      rclk;
   reg                      ren;
   wire [FIFO_WIDTH-1:0]    rdata;
   wire                     rempty;
   wire [LOG2_FIFO_DEPTH:0] rlevel;
   reg                      wsrst;
   reg                      wclk;
   reg                      wen;
   reg  [FIFO_WIDTH-1:0]    wdata;
   wire                     wfull;
   wire [LOG2_FIFO_DEPTH:0] wlevel;

   reg                      arst;

   // ref signals

   // counter
   integer                  rcpt, wcpt;

   //----------------------------------------------------------------
   // DUT
   //----------------------------------------------------------------
   dclkfifolut
   #(
      .LOG2_FIFO_DEPTH (LOG2_FIFO_DEPTH),
      .FIFO_WIDTH      (FIFO_WIDTH)
   )
   DUT
   (
      .rsrst   (rsrst),
      .rclk    (rclk),
      .ren     (ren),
      .rdata   (rdata),
      .rempty  (rempty),
      .rlevel  (rlevel),
      .wsrst   (wsrst),
      .wclk    (wclk),
      .wen     (wen),
      .wdata   (wdata),
      .wfull   (wfull),
      .wlevel  (wlevel)
   );

   //----------------------------------------------------------------
   // Clock and Reset Generation
   //----------------------------------------------------------------
   initial begin
      rclk        = 0;
      wclk        = 0;
      arst        = 1;
      #10.2 arst  = 1;
      #13.4 arst  = 0;
   end

   always
     #2 rclk = !rclk;

   always
     #3 wclk = !wclk;

   always @(posedge rclk) begin
      rsrst <= arst;
   end

   always @(posedge wclk) begin
      wsrst <= arst;
   end


   //----------------------------------------------------------------
   // Value Change Dump
   //----------------------------------------------------------------
   initial  begin
      $dumpfile ("dclkfifolut_tb.vcd");
      $dumpvars;
   end

   //----------------------------------------------------------------
   // Test vectors
   //----------------------------------------------------------------
   initial
     #300 $finish;

   always @(posedge rclk)
     if(rsrst) begin
        rcpt <= 0;
     end
     else begin
        rcpt <= rcpt + 1;
     end

   always @(posedge wclk)
     if(wsrst) begin
        wcpt <= 0;
     end
     else begin
        wcpt <= wcpt + 1;
     end

   // write the fifo
   always @(wcpt) begin
      // write first pattern
      if ((wcpt >= 1) && (wcpt<=7)) begin
         wen    = 1'b1;
         wdata  = wcpt;
      end

      else if (wcpt == 9) begin
         wen    = 1'b1;
         wdata  = 8;
      end

      // try to overflow the fifo with one more write
      else if (wcpt == 10) begin
         wen    = 1'b1;
         wdata  = wcpt;
      end

      // write another pattern after corruption attempt
      else if ((wcpt >= 21) && (wcpt<=28)) begin
         wen    = 1'b1;
         wdata  = wcpt;
      end

      else begin
         wen    = 1'b0;
         wdata  = 0;
      end
   end

   // read the fifo
   always @(rcpt) begin
      if ((rcpt >= 21) && (rcpt<=27)) begin
        ren  = 1'b1;
      end

      else if (rcpt == 29) begin
        ren = 1'b1;
      end

      // try to overflow the fifo with one more read
      else if (rcpt == 30) begin
        ren = 1'b1;
      end

      // read continously the fifo
      else if (rcpt >= 33) begin
         ren = ~rempty;
      end

      else begin
        ren  = 1'b0;
      end
   end

   //----------------------------------------------------------------
   // Checks
   //----------------------------------------------------------------

   reg ren_reg;
   reg [7:0] read_state = 1;

   always @(posedge rclk) begin
      ren_reg <= ren && !rempty;
   end

   always @(posedge rclk) begin
      if (ren_reg == 1'b1) begin
         case (read_state)
            8'h1:  read_state <= 8'h2;
            8'h2:  read_state <= 8'h3;
            8'h3:  read_state <= 8'h4;
            8'h4:  read_state <= 8'h5;
            8'h5:  read_state <= 8'h6;
            8'h6:  read_state <= 8'h7;
            8'h7:  read_state <= 8'h8;
            8'h8:  read_state <= 8'h15;
            8'h15: read_state <= 8'h16;
            8'h16: read_state <= 8'h17;
            8'h17: read_state <= 8'h18;
            8'h18: read_state <= 8'h19;
            8'h19: read_state <= 8'h1a;
            8'h1a: read_state <= 8'h1b;
            8'h1b: read_state <= 8'h1c;
            8'h1c: read_state <= 8'h1c;
         endcase
      end
   end

   always @(posedge rclk) begin
      if (ren_reg == 1'b1) begin
         if (rdata != read_state) begin
            $display("Error: bad data read: 0x%02x, ref: 0x%2x", rdata, read_state);
         end
         else begin
            $display("Ok, data read: 0x%02x, ref: 0x%2x", rdata, read_state);
         end
      end
   end

endmodule
