module fsm_loop_err0 (
    input logic clk,
    input logic rst_n,
    output logic [7:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        (* veriparse_no_unroll *)
        repeat (4) q <= q + 8'd1;
        @(posedge clk);
    end

endmodule
