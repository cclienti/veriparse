module fsm_decode_err2 (input logic clk, input logic rst_n, output logic busy);
    (* veriparse_fsm *)
    initial begin
        busy = 1'b0;
        @(posedge clk);
        busy = 1'b1;
        @(posedge clk);
        @(posedge clk);
        busy = 1'b0;
    end
endmodule
