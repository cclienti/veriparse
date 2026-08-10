module fsm_hint_err1 (
    input logic clk,
    input logic rst_n,
    output logic a,
    output logic b
);

    (* veriparse_fsm, veriparse_prefix = "__fsm1" *)
    initial begin
        a <= 1'b0;
        @(posedge clk);
        a <= 1'b1;
        @(posedge clk);
    end

    (* veriparse_fsm *)
    initial begin
        b <= 1'b0;
        @(posedge clk);
        b <= 1'b1;
        @(posedge clk);
    end

endmodule
