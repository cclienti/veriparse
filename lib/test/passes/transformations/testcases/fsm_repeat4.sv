module fsm_repeat4 (
    input logic clk,
    input logic rst_n,
    input int n,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        done <= 1'b0;
        @(posedge clk);
        (* veriparse_no_unroll *)
        repeat (n) @(posedge clk);
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
