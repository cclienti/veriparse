module fsm_repeat2 (
    input logic clk,
    input logic rst_n,
    input logic [3:0] n,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        done <= 1'b0;
        @(posedge clk);
        repeat (n) @(posedge clk);
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
