// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2025-2026 Christophe Clienti
//
// RMII MAC receiver (Fast Ethernet), behavioural form — adapted from
// verilog-ip's rmii_mac_rx and relicensed by its author. The three-process
// FSM of the reference (state register, next-state always_comb, Moore
// output decode) is rewritten as one marked imperative process: each
// original state is a wait, the Moore decode becomes commit-on-entry
// nonblocking assignments — cycle-for-cycle the same observable timing —
// and DROP/ERROR recovery is the loop's continue. The input
// synchronizer, SFD detector and tdata tap stay the plain RTL they are.
module rmii_mac_rx (
    input logic        clock,      // Clock signal, 50 MHz
    input logic        srst,       // Synchronous reset, active high

    input logic [1:0]  rxd,        // RXD is the data received from the PHY, 2 bits for RMII
    input logic        rxen,       // RXEN is high when the PHY is sending data

    output logic       axi_tvalid, // Indicates that the AXI stream data is valid
    output logic       axi_tlast,  // Indicates the last data in the frame
    output logic [1:0] axi_tdata,  // Data to be sent on the AXI stream
    output logic       axi_tuser,  // Indicates an error in the frame
    input logic        axi_tready  // Indicates that the AXI stream is ready to accept data
);

    //-------------------------------------------
    // Input synchronization
    //-------------------------------------------
    localparam int SYNC_DEPTH = 5;

    logic [1:0]    rxd_d [0:SYNC_DEPTH-1];
    logic          rxen_d [0:SYNC_DEPTH-1];

    always @(posedge clock) begin
        if (srst) begin
            for (int i = 0; i < SYNC_DEPTH; i++) begin
                rxd_d[i]  <= 2'b00;
                rxen_d[i] <= 1'b0;
            end
        end
        else begin
            for (int i = 1; i < SYNC_DEPTH; i++) begin
                rxd_d[i]  <= rxd_d[i-1];
                rxen_d[i] <= rxen_d[i-1];
            end
            rxd_d[0]  <= rxd;
            rxen_d[0] <= rxen;
        end
    end

    //-------------------------------------------
    // Detect start/end of frame
    //-------------------------------------------
    logic rxen_falling;
    logic rxen_rising;
    assign rxen_falling = rxen_d[1] && !rxen_d[0];
    assign rxen_rising = !rxen_d[1] && rxen_d[0];

    //--------------------------------------------
    // SFD detection logic
    //--------------------------------------------
    logic sfd_detected;
    // Should be 0xD5 but we want also to detect 0xD6 to handle the
    // case where the PHY sends a 0xD6 instead of 0xD5.
    always_ff @(posedge clock) begin
        if (srst) begin
            sfd_detected <= 1'b0;
        end else begin
            // Check for SFD: 0xD5 (11010101) or 0xD6 (11010110)
            sfd_detected <= (rxd_d[1][1] & rxd_d[1][0]) &
                            (rxd_d[2][1] ^ rxd_d[2][0]) &
                            (rxd_d[3][1] ^ rxd_d[3][0]) &
                            (rxd_d[4][1] ^ rxd_d[4][0]);
        end
    end

    //-------------------------------------------
    // AXI stream control, imperatively: the reference's IDLE, START,
    // DATA, LAST, ERROR and DROP states are the waits below, its Moore
    // decode the commits entering them.
    //-------------------------------------------
    (* veriparse_fsm, veriparse_reset = "srst" *)
    initial begin
        axi_tvalid <= 1'b0;
        axi_tlast  <= 1'b0;
        axi_tuser  <= 1'b0;
        @(posedge clock);
        forever begin
            begin : IDLE
                @(posedge clock);
                while (!rxen_rising) @(posedge clock);
            end
            begin : START
                @(posedge clock);
                while (!(rxen_falling || sfd_detected)) @(posedge clock);
            end
            if (rxen_falling || !axi_tready) begin : DROP
                @(posedge clock);
                continue;
            end
            begin : DATA
                axi_tvalid <= 1'b1;
                @(posedge clock);
                while (axi_tready && !rxen_falling) @(posedge clock);
            end
            if (axi_tready) begin : LAST
                axi_tlast <= 1'b1;
                @(posedge clock);
            end
            if (!axi_tready) begin : ERROR
                axi_tlast <= 1'b1;
                axi_tuser <= 1'b1;
                @(posedge clock);
                while (!axi_tready) @(posedge clock);
                axi_tvalid <= 1'b0;
                axi_tlast  <= 1'b0;
                axi_tuser  <= 1'b0;
                continue;
            end
            axi_tvalid <= 1'b0;
            axi_tlast  <= 1'b0;
        end
    end

    //-------------------------------------------
    // Manage TData
    //-------------------------------------------
    assign axi_tdata = rxd_d[SYNC_DEPTH-3];

endmodule
