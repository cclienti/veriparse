module fsm_branch1 (
    input logic clk,
    input logic rst_n,
    input logic skip,
    input logic [3:0] d,
    output logic [3:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        if (!skip) begin
            q <= d;
            @(posedge clk);
        end
        q <= q + 4'd1;
        @(posedge clk);
    end

endmodule
