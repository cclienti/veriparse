module fsm_forever0 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] d,
    output logic [7:0] q,
    output logic phase,
    output logic armed
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        phase <= 1'b0;
        armed <= 1'b0;
        @(posedge clk);
        armed <= 1'b1;
        @(posedge clk);
        forever begin
            phase <= 1'b1;
            q <= q + d;
            @(posedge clk);
            phase <= 1'b0;
            q <= q ^ 8'h0f;
            @(posedge clk);
        end
    end

endmodule
