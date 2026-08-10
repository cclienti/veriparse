// ADR-0014 Appendix A.1: the behavioural RS232 transmitter. Perpetual (§2):
// a one-time prologue, then forever — wait for send, start bit, eight data
// bits (the for unrolls into eight states, each reading its constant
// index), stop bit, and the back-to-back frame case: send still high at the
// end of the stop bit restarts on that edge, no idle cycle.
module rs232_tx #(parameter int BAUD_DIV = 4) (
     input logic       clk, rst_n,
     input logic [7:0] data,
     input logic       send,
     output logic      tx, busy
);

    (* veriparse_fsm *)
    initial begin
        tx <= 1'b1; busy <= 1'b0;
        @(posedge clk);
        forever begin
            begin : WAIT_SEND
                if (!send) begin                  // one commit per path:
                    busy <= 1'b0;                 // busy only falls if we
                    while (!send) @(posedge clk); // really stop
                end
            end
            busy <= 1'b1;
            begin : START
                tx <= 1'b0;
                (* veriparse_no_unroll *)
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
