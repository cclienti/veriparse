module fsm_sysfunc_err0 (
    input logic clk,
    input logic rst_n,
    output logic [7:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        q <= $random;
        @(posedge clk);
    end

endmodule
