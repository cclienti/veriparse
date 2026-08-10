module fsm_repeat_err0 (
    input logic clk,
    input logic rst_n,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        done <= 1'b0;
        @(posedge clk);
        (* veriparse_no_unroll *)
        repeat (3) begin
            @(posedge clk);
            (* veriparse_no_unroll *)
            repeat (4) @(posedge clk);
        end
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
