module fsm_decode_err15 (input logic clk, input logic rst_n, input logic d,
                         output logic x);
    logic r;
    generate
        if (1) begin : g
            always_ff @(posedge clk) r <= d;
        end
    endgenerate
    (* veriparse_fsm *)
    initial begin
        x = 1'b0;
        @(posedge clk);
        x = r;
        @(posedge clk);
        x = 1'b0;
    end
endmodule
