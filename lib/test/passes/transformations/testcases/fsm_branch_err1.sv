module fsm_branch_err1 (
    input logic clk,
    input logic rst_n,
    input logic c,
    output logic q
);

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
        if (c) @(posedge clk);
        @(posedge clk);
        q <= 1'b1;
    end

endmodule
