module fsm_for_err2 (
    input logic clk,
    input logic rst_n,
    output logic [7:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        (* veriparse_no_unroll *)
        for (k = 4'd0; k < 4'd3; k = k + 4'd1) begin
            q <= q + 8'd1;
            @(posedge clk);
        end
        @(posedge clk);
    end

endmodule
