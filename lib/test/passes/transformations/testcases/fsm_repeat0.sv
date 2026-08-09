module fsm_repeat0 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] d,
    output logic [7:0] q,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        done <= 1'b0;
        @(posedge clk);
        q <= d;
        (* veriparse_no_unroll *)
        repeat (4) @(posedge clk);
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
