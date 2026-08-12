module fsm_decode_err12 (input logic clk, input logic rst_n, input logic mode,
                         output logic sel);
    (* veriparse_fsm *)
    initial begin
        if (mode) sel = 1'b1;
        else      sel = 1'b0;
        @(posedge clk);
        sel = 1'b0;
        @(posedge clk);
        sel = 1'b0;
    end
endmodule
