module fsm_multidrive_err2 (
    input logic clk,
    input logic rst_n,
    input logic a,
    output logic [7:0] q
);

    generate
        if (1) begin : g
            always @(posedge clk) if (a) q <= 8'd5;
        end
    endgenerate

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        q <= 8'd1;
        @(posedge clk);
    end

endmodule
