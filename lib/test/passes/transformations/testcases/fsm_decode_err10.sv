module fsm_decode_err10 (input logic clk, input logic rst_n,
                         output wire logic y);
    (* veriparse_fsm *)
    initial begin
        y = 1'b0;
        @(posedge clk);
        y = 1'b1;
        @(posedge clk);
        y = 1'b0;
    end
endmodule
