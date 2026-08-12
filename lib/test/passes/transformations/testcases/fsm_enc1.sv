module fsm_enc1 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] d,
    output logic [7:0] q,
    output logic busy
);

    (* veriparse_fsm, veriparse_encoding = "output" *)
    initial begin
        q    <= '0;
        busy = 1'b0;
        @(posedge clk);
        busy = 1'b1;
        q <= d;
        (* veriparse_no_unroll *)
        repeat (0) begin
            busy = 1'b1;
            @(posedge clk);
        end
        @(posedge clk);
        busy = 1'b0;
    end

endmodule
