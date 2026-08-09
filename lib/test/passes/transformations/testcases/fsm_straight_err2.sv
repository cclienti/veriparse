module fsm_straight_err2 (
    input logic clk,
    input logic rst_n,
    output logic [3:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= q + 4'd1;
        @(posedge clk);
        q <= 4'd2;
        @(posedge clk);
    end

endmodule
