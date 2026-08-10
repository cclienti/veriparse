module fsm_jump2 (
    input logic clk,
    input logic rst_n,
    input logic stop,
    input logic kill,
    output logic [7:0] q,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        done <= 1'b0;
        @(posedge clk);
        while (!stop) begin
            (* veriparse_no_unroll *)
            repeat (5) begin
                @(posedge clk);
                if (kill) break;
            end
            if (kill) continue;
            q <= q + 8'd1;
            @(posedge clk);
        end
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
