module fsm_systask_err0 (
    input logic clk,
    input logic rst_n,
    output logic q
);

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
        @(posedge clk);
        $display("hello");
        q <= 1'b1;
        @(posedge clk);
    end

endmodule
