module fsm_enc0 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] d,
    output logic [7:0] q,
    output logic busy,
    output logic [1:0] phase
);

    (* veriparse_fsm, veriparse_encoding = "output" *)
    initial begin
        q     <= '0;
        busy  = 1'b0;
        phase = 2'd0;
        @(posedge clk);
        busy  = 1'b1;
        phase = 2'd1;
        q <= d;
        @(posedge clk);
        busy  = 1'b1;
        phase = 2'd1;
        q <= q + 8'd1;
        @(posedge clk);
        busy  = 1'b1;
        phase = 2'd2;
        q <= q ^ 8'h55;
        @(posedge clk);
        busy  = 1'b0;
        phase = 2'd3;
    end

endmodule
