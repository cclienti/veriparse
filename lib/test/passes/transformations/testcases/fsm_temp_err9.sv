module fsm_temp_err9 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] a,
    output logic [7:0] q
);

    logic [3:0] i;

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        (* veriparse_no_unroll *)
        for (i = 4'd0; i < 4'd2; i = i + 4'd1) begin
            begin
                logic [7:0] i;
                i = a + 8'd1;
                q <= i;
            end
            @(posedge clk);
        end
        @(posedge clk);
    end

endmodule
