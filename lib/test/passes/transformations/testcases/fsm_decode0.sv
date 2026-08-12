module fsm_decode0 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] d,
    output logic [7:0] q,
    output logic busy,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        q    <= '0;
        busy = 1'b0;
        done = 1'b0;
        @(posedge clk);
        busy = 1'b1;
        done = 1'b0;
        q <= d;
        @(posedge clk);
        busy = 1'b1;
        done = 1'b0;
        q <= q + 8'd1;
        @(posedge clk);
        busy = 1'b0;
        done = 1'b1;
    end

endmodule
