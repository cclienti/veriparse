module fsm_decode_err1 (input logic clk, input logic rst_n, output logic y);
    (* veriparse_fsm *)
    initial begin
        y = 1'b0;
        @(posedge clk);
        y <= 1'b1;
        @(posedge clk);
    end
endmodule
