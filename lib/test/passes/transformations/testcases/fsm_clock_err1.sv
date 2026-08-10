module fsm_clock_err1 (
    input logic clk,
    input logic rst_n,
    input logic a,
    output logic q
);

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
        @(posedge clk);
        q <= 1'b1;
        @(a);
    end

endmodule
