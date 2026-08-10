module fsm_disable_err0 (
    input logic clk,
    input logic rst_n,
    output logic q
);

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
        @(posedge clk);
        begin : L
            q <= 1'b1;
            disable L;
        end
        @(posedge clk);
    end

endmodule
