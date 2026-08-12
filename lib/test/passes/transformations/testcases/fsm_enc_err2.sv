module fsm_enc_err2 (input logic clk, input logic rst_n, input logic [7:0] d,
                     output logic [7:0] r, output logic [7:0] v);
    (* veriparse_fsm, veriparse_encoding = "output" *)
    initial begin
        r <= '0;
        v = 8'd0;
        @(posedge clk);
        r <= d;
        v = 8'd1;
        @(posedge clk);
        v = r;
        @(posedge clk);
        v = 8'd0;
    end
endmodule
