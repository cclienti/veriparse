module fsm_while_err0 (
    input logic clk,
    input logic rst_n,
    input logic c,
    input logic d,
    output logic q
);

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
        @(posedge clk);
        while (c) begin
            if (d) @(posedge clk);
        end
        q <= 1'b1;
        @(posedge clk);
    end

endmodule
