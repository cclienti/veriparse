module fsm_clock_err2 (
    input logic clk,
    input logic rst_n,
    output logic q
);

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
        @(posedge clk);
        q <= 1'b1;
        @(negedge clk);
    end

endmodule
