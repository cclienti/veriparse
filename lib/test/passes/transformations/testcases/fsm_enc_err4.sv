module fsm_enc_err4 (input logic clk, input logic rst_n, output logic busy);
    (* veriparse_fsm, veriparse_encoding = "output" *)
    initial begin
        busy = 1'b0;
        @(posedge clk);
        busy = ^(2'd3 + 2'd1);
        @(posedge clk);
        busy = 1'b0;
    end
endmodule
