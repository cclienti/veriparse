module fsm_fork_err0 (
    input logic clk,
    input logic rst_n,
    output logic q,
    output logic r
);

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
        @(posedge clk);
        fork
            q <= 1'b1;
            r <= 1'b1;
        join
        @(posedge clk);
    end

endmodule
