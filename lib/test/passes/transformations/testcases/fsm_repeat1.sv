module fsm_repeat1 (
    input logic clk,
    input logic rst_n,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        done <= 1'b0;
        @(posedge clk);
        (* veriparse_no_unroll *)
        repeat (1) @(posedge clk);
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
