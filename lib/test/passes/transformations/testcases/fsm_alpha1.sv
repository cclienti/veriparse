module fsm_alpha1 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] a,
    output logic [7:0] q,
    output logic [7:0] r
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        r <= '0;
        @(posedge clk);
        begin
            logic [7:0] r;
            r = a + 8'd1;
            q <= r;
        end
        @(posedge clk);
        r <= 8'd5;
    end

endmodule
