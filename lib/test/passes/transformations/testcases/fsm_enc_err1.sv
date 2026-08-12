module fsm_enc_err1 (input logic clk, input logic rst_n, input logic m,
                     output logic sel);
    logic m_r;
    (* veriparse_fsm, veriparse_encoding = "output" *)
    initial begin
        m_r <= 1'b0;
        sel = 1'b0;
        @(posedge clk);
        m_r <= m;
        sel = 1'b0;
        @(posedge clk);
        if (m_r) sel = 1'b1;
        else     sel = 1'b0;
        @(posedge clk);
        sel = 1'b0;
    end
endmodule
