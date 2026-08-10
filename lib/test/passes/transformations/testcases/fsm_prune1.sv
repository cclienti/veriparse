module fsm_prune1 (
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
        repeat (n) @(posedge clk);
        q <= 8'd2;
        repeat (n) @(posedge clk);
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
