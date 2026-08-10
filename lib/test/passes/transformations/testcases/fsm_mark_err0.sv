module fsm_mark_err0 (
    input logic clk,
    input logic rst_n,
    input logic a,
    output logic q
);

    (* veriparse_fsm *)
    assign q = a;

endmodule
