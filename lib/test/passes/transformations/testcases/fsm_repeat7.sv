module fsm_repeat7 (
    input logic clk,
    input logic rst_n,
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
            repeat (3) begin
                (* veriparse_no_unroll *)
                repeat (4) @(posedge clk);
                q <= q + 8'd1;
                @(posedge clk);
            end
            (* veriparse_no_unroll *)
            repeat (5) @(posedge clk);
            q <= q + 8'd16;
            @(posedge clk);
        end
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
