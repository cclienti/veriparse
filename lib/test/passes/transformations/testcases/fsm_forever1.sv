module fsm_forever1 (
    input logic clk,
    input logic rst_n,
    input logic stop,
    output logic [7:0] q,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        done <= 1'b0;
        @(posedge clk);
        forever begin
            @(posedge clk);
            if (stop) break;
            q <= q + 8'd1;
        end
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
