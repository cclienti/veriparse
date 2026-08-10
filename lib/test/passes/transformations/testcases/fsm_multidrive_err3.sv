module drv_child (
    input logic clk,
    output logic [7:0] out
);

    always_ff @(posedge clk) out <= 8'd3;

endmodule

module fsm_multidrive_err3 (
    input logic clk,
    input logic rst_n,
    output logic [7:0] q
);

    drv_child u_child (.clk(clk), .out(q));

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        q <= 8'd1;
        @(posedge clk);
    end

endmodule
