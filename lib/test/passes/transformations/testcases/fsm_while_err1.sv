module fsm_while_err1 (
    input logic clk,
    input logic rst_n,
    input logic c,
    output logic q
);

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
        @(posedge clk);
        while (c) q <= 1'b1;
        @(posedge clk);
    end

endmodule
