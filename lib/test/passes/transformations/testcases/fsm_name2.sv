module fsm_name2 (
    input logic clk,
    input logic rst_n,
    output logic [7:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        begin : PHASE0
            @(posedge clk);
            q <= 8'd1;
        end
        @(posedge clk);
    end

endmodule
