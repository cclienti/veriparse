module fsm_sysfunc_err1 (
    input logic clk,
    input logic rst_n,
    output logic [7:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        if ($random & 1) q <= 8'd1;
        @(posedge clk);
    end

endmodule
