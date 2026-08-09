module fsm_jump0 (
    input logic clk,
    input logic rst_n,
    input logic stop,
    input logic skip,
    input logic [7:0] d,
    output logic [7:0] q,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        done <= 1'b0;
        @(posedge clk);
        while (1'b1) begin
            @(posedge clk);
            if (stop) break;
            if (skip) continue;
            q <= q + d;
        end
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
