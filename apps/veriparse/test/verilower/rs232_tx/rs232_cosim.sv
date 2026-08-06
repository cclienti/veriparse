// -----------------------------------------------------------------------------
// ADR-0014 — RS232 TX: behavioral source (A.1) against the generated FSM (A.2)
//
//   $ verilator --binary --timing --top-module tb -Wno-CASEINCOMPLETE \
//         rs232_cosim.sv -o a.out && ./obj_dir/a.out
//
// REQUIRED: verilator >= 5.050 (§11). Earlier versions execute the `<=` of an
// initial block as `=` (INITIALDLY): the reference then commits its values in
// the active region of the same edge and a posedge comparator sees it one
// cycle early. 5.050 implements the IEEE semantics.
//
// The bench is entirely on posedge: stimuli written with `<=`, comparator in
// the active region, so no race between the bench and the DUTs — and the
// convention stays valid for a post-P&R netlist, where mid-cycle sampling
// would assume margins that real delays can violate.
//
// RESERVATION: no alignment offset is applied although §11 requires one.
// The bench only compares correctly because send=0 during the 3 reset
// cycles, which keeps both models idle and aligned by accident.
// Releasing reset with send already high would expose the offset.
//
// The FSM is the UNROLLED machine the revised ADR specifies: the `for` of the
// DATA block describes 8 states (DATA_0..DATA_7), not an index counter. A
// counter machine (idx) would be I/O-equivalent but it is NOT the specified
// output — the pass mirrors the source's control structure (§11.1).
// -----------------------------------------------------------------------------

// ---------- A.1: the behavioral source ----------
// `tx` and `busy` are module outputs: registers, hence `<=`.
// `i` is the `for` step: consumed by unrolling, never a register.
module rs232_tx_beh #(parameter int BAUD_DIV = 4) (
     input logic       clk, rst_n, // rst_n is NOT read: annotation (§5)
     input logic [7:0] data,
     input logic  send,
     output logic tx, busy
);
    (* veriparse_fsm *)
    initial begin
        tx <= 1'b1; busy <= 1'b0;                        // init segment -> reset
        @(posedge clk);
        forever begin
            begin : WAIT_SEND
                if (!send) begin                 // one commit per path:
                    busy <= 1'b0;                // busy only falls if we
                    while (!send) @(posedge clk);// really stop
                end
            end
            busy <= 1'b1;
            begin : START
                tx <= 1'b0;
                (* veriparse_no_unroll *)      // rolled: one counter state
                repeat (BAUD_DIV) @(posedge clk);
            end
            begin : DATA
                for (int i = 0; i < 8; i = i + 1) begin
                    tx <= data[i];
                    (* veriparse_no_unroll *)
                    repeat (BAUD_DIV) @(posedge clk);
                end
            end
            begin : STOP
                tx <= 1'b1;
                (* veriparse_no_unroll *)
                repeat (BAUD_DIV) @(posedge clk);
            end
        end
    end
endmodule

// ---------- A.2: the FSM the ADR specifies ----------
// 11 states: the `for` is unrolled into 8 DATA_k states, each a counter
// state for its `repeat` (§7.2). The indices are constants substituted by
// the unrolling — no index register, no i+1 substitution (§6.1).
module rs232_tx_fsm #(parameter int BAUD_DIV = 4) (
     input logic       clk, rst_n,
     input logic [7:0] data,
     input logic  send,
     output logic tx, busy
);
    localparam [3:0] S_WAIT_SEND = 4'd0;
    localparam [3:0] S_START     = 4'd1;
    localparam [3:0] S_DATA_0    = 4'd2;
    localparam [3:0] S_DATA_1    = 4'd3;
    localparam [3:0] S_DATA_2    = 4'd4;
    localparam [3:0] S_DATA_3    = 4'd5;
    localparam [3:0] S_DATA_4    = 4'd6;
    localparam [3:0] S_DATA_5    = 4'd7;
    localparam [3:0] S_DATA_6    = 4'd8;
    localparam [3:0] S_DATA_7    = 4'd9;
    localparam [3:0] S_STOP      = 4'd10;

    localparam int   CW = $clog2(BAUD_DIV);          // countdown width
    localparam logic [CW-1:0] CNT_TOP = CW'(BAUD_DIV - 1);

    logic [3:0]               state;
    logic [CW-1:0]            cnt;

    always_ff @(posedge clk) begin
        if (!rst_n) begin
            tx    <= 1'b1;
            busy  <= 1'b0;
            state <= S_WAIT_SEND;
        end
        else case (state)

                 S_WAIT_SEND:
                     if (send) begin
                         busy  <= 1'b1;
                         tx    <= 1'b0;
                         cnt   <= CNT_TOP;
                         state <= S_START;
                     end

                 S_START:
                     if (cnt == 0) begin
                         tx <= data[0]; cnt <= CNT_TOP; state <= S_DATA_0;
                     end
                     else cnt <= cnt - 1'b1;

                 S_DATA_0:
                     if (cnt == 0) begin
                         tx <= data[1]; cnt <= CNT_TOP; state <= S_DATA_1;
                     end
                     else cnt <= cnt - 1'b1;

                 S_DATA_1:
                     if (cnt == 0) begin
                         tx <= data[2]; cnt <= CNT_TOP; state <= S_DATA_2;
                     end
                     else cnt <= cnt - 1'b1;

                 S_DATA_2:
                     if (cnt == 0) begin
                         tx <= data[3]; cnt <= CNT_TOP; state <= S_DATA_3;
                     end
                     else cnt <= cnt - 1'b1;

                 S_DATA_3:
                     if (cnt == 0) begin
                         tx <= data[4]; cnt <= CNT_TOP; state <= S_DATA_4;
                     end
                     else cnt <= cnt - 1'b1;

                 S_DATA_4:
                     if (cnt == 0) begin
                         tx <= data[5]; cnt <= CNT_TOP; state <= S_DATA_5;
                     end
                     else cnt <= cnt - 1'b1;

                 S_DATA_5:
                     if (cnt == 0) begin
                         tx <= data[6]; cnt <= CNT_TOP; state <= S_DATA_6;
                     end
                     else cnt <= cnt - 1'b1;

                 S_DATA_6:
                     if (cnt == 0) begin
                         tx <= data[7]; cnt <= CNT_TOP; state <= S_DATA_7;
                     end
                     else cnt <= cnt - 1'b1;

                 S_DATA_7:
                     if (cnt == 0) begin
                         tx <= 1'b1; cnt <= CNT_TOP; state <= S_STOP;
                     end
                     else cnt <= cnt - 1'b1;

                 S_STOP:                          // decides in the cycle that
                     if (cnt == 0) begin          // ends the stop bit: a frame
                         if (send) begin          // already requested restarts
                             busy <= 1'b1; tx <= 1'b0; cnt <= CNT_TOP; state <= S_START;
                         end                      // on THIS edge, no idle cycle
                         else begin
                             busy <= 1'b0; state <= S_WAIT_SEND;
                         end
                     end
                     else cnt <= cnt - 1'b1;

             endcase
    end
endmodule

// ---------- the bench: everything on posedge ----------
// Stimuli written with `<=` at the rising edge: the DUTs, which read in the
// active region at that same edge, therefore see the old value, and the new
// one at the next edge. No race between the bench and the DUTs.
// Comparator in the active region at the rising edge: it reads the values
// established by the NBA of the previous edge, same convention for both DUTs.
module tb;
    logic clk = 0, rst_n = 0, send = 0;
    logic [7:0] data = 8'hA5;
    logic       tx_b, busy_b, tx_f, busy_f;
    int         errors = 0, checked = 0, rnd = 0;
    bit         checking = 0;

    always #5 clk = ~clk;

    rs232_tx_beh #(.BAUD_DIV(4)) u_beh (.clk, .rst_n, .data, .send, .tx(tx_b), .busy(busy_b));
    rs232_tx_fsm #(.BAUD_DIV(4)) u_fsm (.clk, .rst_n, .data, .send, .tx(tx_f), .busy(busy_f));

    always @(posedge clk) if (checking) begin
        checked++;
        if (tx_b !== tx_f || busy_b !== busy_f) begin
            errors++;
            if (errors < 12)
                $display("MISMATCH t=%0t  tx beh=%b fsm=%b   busy beh=%b fsm=%b",
                         $time, tx_b, tx_f, busy_b, busy_f);
        end
    end

    initial begin
        $dumpfile("rs232.vcd");
        $dumpvars(0, tb);
    end

    initial begin
        repeat (3) @(posedge clk);
        rst_n <= 1'b1;  checking <= 1'b1;

        // 1. a single isolated frame
        @(posedge clk); send <= 1'b1;
        @(posedge clk); send <= 1'b0;
        repeat (60) @(posedge clk);

        // 2. back-to-back: send held across the frame boundary
        send <= 1'b1;
        repeat (140) @(posedge clk);
        send <= 1'b0;
        repeat (60) @(posedge clk);

        // 3. random stimulus
        for (int k = 0; k < 400; k++) begin
            @(posedge clk);
            rnd = $random;
            if (k % 7  == 0) send <= rnd[0];
            if (k % 13 == 0) data <= rnd[15:8];
        end
        repeat (60) @(posedge clk);

        $display("checked=%0d errors=%0d", checked, errors);
        if (errors == 0) $display("EQUIVALENT"); else $display("DIVERGED");
        $finish;
    end
endmodule
