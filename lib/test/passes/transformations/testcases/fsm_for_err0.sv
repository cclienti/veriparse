module fsm_for_err0 (
    input logic clk,
    input logic rst_n,
    output logic [7:0] q
);

    logic [3:0] i;
    logic [3:0] j;

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        (* veriparse_no_unroll *)
        for (i = 4'd0; i < 4'd3; j = j + 4'd1) begin
            q <= q + 8'd1;
            @(posedge clk);
        end
        @(posedge clk);
    end

endmodule
