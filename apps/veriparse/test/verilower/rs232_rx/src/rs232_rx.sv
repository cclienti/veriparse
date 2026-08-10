// The RS232 receiver the ADR's §8 points at: perpetual, and the jumps do
// the protocol's error handling — `continue` drops back to hunting for a
// start bit when the low level turns out to be a glitch at mid-bit, and
// again when the stop bit fails to be high (framing error). The eight data
// bits shift in LSB-first (the unmarked repeat unrolls into eight copies,
// each holding its rolled baud countdown); `valid` pulses one cycle per
// good frame, cleared at the top of every hunt.
module rs232_rx #(parameter int BAUD_DIV = 4) (
     input logic       clk, rst_n,
     input logic       rx,
     output logic [7:0] data,
     output logic      valid
);

    (* veriparse_fsm *)
    initial begin
        data <= '0; valid <= 1'b0;
        @(posedge clk);
        forever begin
            valid <= 1'b0;
            begin : HUNT
                while (rx) @(posedge clk);        // wait for the start edge
            end
            begin : ALIGN
                (* veriparse_no_unroll *)
                repeat (BAUD_DIV / 2) @(posedge clk);
                if (rx) continue;                 // a glitch, not a start bit
            end
            begin : BITS
                repeat (8) begin
                    (* veriparse_no_unroll *)
                    repeat (BAUD_DIV) @(posedge clk);
                    data <= {rx, data[7:1]};      // LSB first
                end
            end
            begin : STOP
                (* veriparse_no_unroll *)
                repeat (BAUD_DIV) @(posedge clk);
                if (!rx) continue;                // framing error: drop it
                valid <= 1'b1;
                @(posedge clk);
            end
        end
    end

endmodule
