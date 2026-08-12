module fsm_decode_err3 (input logic clk, input logic rst_n, output logic busy);
    (* veriparse_fsm *)
    initial begin
        busy = 1'b0;
        @(posedge clk);
        busy = 1'b1;
        (* veriparse_no_unroll *)
        repeat (4) @(posedge clk);
        busy = 1'b0;
        @(posedge clk);
    end
endmodule
