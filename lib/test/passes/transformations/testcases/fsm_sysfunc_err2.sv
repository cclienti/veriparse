module fsm_sysfunc_err2 (
    input logic clk,
    input logic rst_n,
    output logic [7:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk iff ($random & 1));
        q <= 8'd1;
        @(posedge clk iff ($random & 1));
    end

endmodule
