module fsm_branch_err0 (
    input logic clk,
    input logic rst_n,
    input logic c,
    output logic [3:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        if (c) begin
            q <= 4'd1;
            @(posedge clk);
        end
        else q <= 4'd2;
        q <= 4'd3;
        @(posedge clk);
    end

endmodule
