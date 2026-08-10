module fsm_for_err1 (
    input logic clk,
    input logic rst_n,
    input logic [3:0] i,
    output logic [7:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        (* veriparse_no_unroll *)
        for (i = 4'd0; i < 4'd3; i = i + 4'd1) begin
            q <= q + 8'd1;
            @(posedge clk);
        end
        @(posedge clk);
    end

endmodule
