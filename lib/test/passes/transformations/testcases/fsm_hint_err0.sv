module fsm_hint_err0 (
    input logic clk,
    input logic rst_n,
    output logic q
);

    (* veriparse_fsm, veriparse_encoding = "johnson" *)
    initial begin
        q <= 1'b0;
        @(posedge clk);
        q <= 1'b1;
        @(posedge clk);
    end

endmodule
