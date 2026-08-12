module fsm_decode_err8 (input logic clk, input logic rst_n, input logic a,
                        output logic x);
    wire b;
    assign b = a;
    (* veriparse_fsm *)
    initial begin
        x = 1'b0;
        @(posedge clk);
        x = b;
        @(posedge clk);
        x = 1'b0;
    end
endmodule
