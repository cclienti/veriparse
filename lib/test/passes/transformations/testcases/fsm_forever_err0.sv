module fsm_forever_err0 (
    input logic clk,
    input logic rst_n,
    output logic q
);

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
        @(posedge clk);
        forever q <= ~q;
    end

endmodule
