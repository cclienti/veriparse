module fsm_prune_err0 (
    input logic clk,
    input logic rst_n,
    output logic [7:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        if ($random & 1) begin
            q <= 8'd1;
            @(posedge clk);
        end
        q <= 8'd2;
        @(posedge clk);
    end

endmodule
