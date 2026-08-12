module fsm_decode_err9 (input logic clk, input logic rst_n, input logic d,
                        output logic x);
    logic r;
    always_ff @(posedge clk) r <= d;
    (* veriparse_fsm *)
    initial begin
        x = 1'b0;
        @(posedge clk);
        x = r;
        @(posedge clk);
        x = 1'b0;
    end
endmodule
