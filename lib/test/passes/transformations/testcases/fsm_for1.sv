module fsm_for1 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] d,
    output logic [7:0] q,
    output logic [7:0] r,
    output logic done
);

    logic [3:0] i;

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        r <= '0;
        done <= 1'b0;
        @(posedge clk);
        (* veriparse_no_unroll *)
        for (i = 4'd0; i < 4'd2; i = i + 4'd1) begin
            q <= q + d;
            @(posedge clk);
        end
        (* veriparse_no_unroll *)
        for (i = 4'd0; i < 4'd3; i = i + 4'd1) begin
            r <= r + d;
            @(posedge clk);
        end
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
