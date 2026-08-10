module fsm_blocking_err0 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] d,
    output logic [7:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        q = d + 8'd1;
        @(posedge clk);
    end

endmodule
