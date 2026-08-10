module fsm_mixed1 (
    input logic clk,
    input logic rst_n,
    input logic start,
    input logic stop,
    output logic [7:0] q,
    output logic [7:0] r,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        r <= '0;
        done <= 1'b0;
        @(posedge clk);
        while (!start) begin
            (* veriparse_no_unroll *)
            repeat (2) @(posedge clk);
            q <= q + 8'd1;
        end
        (* veriparse_no_unroll *)
        repeat (3) begin
            while (!stop) @(posedge clk);
            r <= r + 8'd16;
            @(posedge clk);
        end
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
