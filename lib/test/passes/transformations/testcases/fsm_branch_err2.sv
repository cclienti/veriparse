module fsm_branch_err2 (
    input logic clk,
    input logic rst_n,
    input logic c,
    input logic [3:0] d,
    output logic [3:0] q,
    output logic [3:0] r
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        if (c) begin
            r <= d;
            @(posedge clk);
        end
        else begin
            q <= d;
            @(posedge clk);
        end
        q <= r + 4'd1;
        @(posedge clk);
    end

endmodule
