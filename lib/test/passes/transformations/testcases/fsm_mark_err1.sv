module fsm_mark_err1 (
    input logic clk,
    input logic rst_n,
    output logic q
);

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
    end

endmodule
