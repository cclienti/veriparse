module fsm_repeat_err2 (
    input logic clk,
    input logic rst_n,
    input logic [3:0] n,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        done <= 1'b0;
        @(posedge clk);
        (* veriparse_no_unroll *)
        repeat (n + 4'd1) @(posedge clk);
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
