module fsm_repeat9 (
    input logic clk,
    input logic rst_n,
    input logic [3:0] n,
    output logic [7:0] q,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        done <= 1'b0;
        @(posedge clk);
        (* veriparse_no_unroll *)
        repeat (2) begin
            (* veriparse_no_unroll *)
            repeat (n) begin
                q <= q + 8'd1;
                @(posedge clk);
            end
            @(posedge clk);
        end
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
