module fsm_delay_err0 (
    input logic clk,
    input logic rst_n,
    output logic q
);

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
        @(posedge clk);
        #5 q <= 1'b1;
        @(posedge clk);
    end

endmodule
