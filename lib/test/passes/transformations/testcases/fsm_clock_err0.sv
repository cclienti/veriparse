module fsm_clock_err0 (
    input logic clk,
    input logic clk2,
    input logic rst_n,
    output logic q
);

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
        @(posedge clk);
        q <= 1'b1;
        @(posedge clk2);
    end

endmodule
