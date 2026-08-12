module fsm_enc_err3 (input logic clk, input logic rst_n, input logic [7:0] d,
                     output logic [7:0] q);
    (* veriparse_fsm, veriparse_encoding = "output" *)
    initial begin
        q <= '0;
        @(posedge clk);
        q <= d;
        @(posedge clk);
    end
endmodule
