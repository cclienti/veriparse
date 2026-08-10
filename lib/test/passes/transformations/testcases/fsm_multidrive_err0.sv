module fsm_multidrive_err0 (
    input logic clk,
    input logic rst_n,
    input logic a,
    output logic [7:0] q
);

    always_ff @(posedge clk)
        if (a) q <= 8'd7;

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        q <= 8'd1;
        @(posedge clk);
    end

endmodule
