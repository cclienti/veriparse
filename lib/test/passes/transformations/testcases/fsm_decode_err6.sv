module fsm_decode_err6 (input logic clk, input logic rst_n,
                        output logic a, output logic b);
    (* veriparse_fsm *)
    initial begin
        a = 1'b0;
        b = 1'b0;
        @(posedge clk);
        a = ~b;
        b = 1'b1;
        @(posedge clk);
        a = 1'b0;
        b = 1'b0;
    end
endmodule
