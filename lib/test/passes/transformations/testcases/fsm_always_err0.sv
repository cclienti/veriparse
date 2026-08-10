module fsm_always_err0 (
    input logic clk,
    input logic rst_n,
    output logic q
);

    (* veriparse_fsm *)
    always begin
        q <= 1'b0;
        @(posedge clk);
        q <= 1'b1;
        @(posedge clk);
    end

endmodule
