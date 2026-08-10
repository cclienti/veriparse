module fsm_wait_err0 (
    input logic clk,
    input logic rst_n,
    input logic a,
    output logic q
);

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
        @(posedge clk);
        wait (a);
        q <= 1'b1;
        @(posedge clk);
    end

endmodule
