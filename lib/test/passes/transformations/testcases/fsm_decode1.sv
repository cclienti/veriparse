module fsm_decode1 (
    input logic clk,
    input logic rst_n,
    input logic mode,
    output logic [7:0] q,
    output logic sel
);

    logic mode_r;

    (* veriparse_fsm *)
    initial begin
        q      <= '0;
        mode_r <= 1'b0;
        sel = 1'b0;
        @(posedge clk);
        mode_r <= mode;
        if (mode) begin
            sel = 1'b1;
            q <= 8'd1;
        end else begin
            sel = 1'b0;
            q <= 8'd2;
        end
        @(posedge clk);
        sel = 1'b0;
        q <= q + 8'd1;
    end

endmodule
