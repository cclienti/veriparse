module fsm_for_err3 (
    input logic clk,
    input logic rst_n,
    input logic [3:0] n,
    output logic [7:0] q,
    output logic done
);

    int i;

    (* veriparse_fsm *)
    initial begin
        q    <= '0;
        done <= 1'b0;
        i    <= 0;
        @(posedge clk);
        for (i = 0; i < n; i = i + 1) begin
            q <= q + 8'd1;
            @(posedge clk);
        end
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
