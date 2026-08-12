module fsm_enc_err5 (input logic clk, input logic rst_n,
                     output logic [16:0] a, output logic [16:0] b);
    (* veriparse_fsm, veriparse_encoding = "output" *)
    initial begin
        a = 17'd0;
        b = 17'd0;
        @(posedge clk);
        a = 17'd1;
        b = 17'd2;
        @(posedge clk);
        a = 17'd0;
        b = 17'd0;
    end
endmodule
