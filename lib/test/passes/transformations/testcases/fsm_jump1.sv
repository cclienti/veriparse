module fsm_jump1 (
    input logic clk,
    input logic rst_n,
    input logic abort,
    input logic skip,
    output logic [7:0] q,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        done <= 1'b0;
        @(posedge clk);
        (* veriparse_no_unroll *)
        repeat (4) begin
            while (1'b1) begin
                @(posedge clk);
                if (abort) break;
                if (skip) continue;
                q <= q + 8'd1;
            end
            q <= q + 8'd16;
            @(posedge clk);
        end
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
