module fsm_decode_err4 (input logic clk, input logic rst_n,
                        output logic [7:0] cnt, output logic [7:0] val);
    (* veriparse_fsm *)
    initial begin
        cnt <= '0;
        val = 8'd0;
        @(posedge clk);
        cnt <= cnt + 8'd1;
        val = cnt;
        @(posedge clk);
        val = 8'd0;
    end
endmodule
