module fsm_reset_err0 (
    input logic clk,
    input logic rst,
    input logic reset,
    output logic q
);

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
        @(posedge clk);
        q <= 1'b1;
        @(posedge clk);
    end

endmodule
