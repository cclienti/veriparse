module fsm_decode_err7 (
    input logic clk,
    input logic rst_n,
    input logic mode,
    output logic sel
);

    (* veriparse_fsm *)
    initial begin
        sel = 1'b0;
        @(posedge clk);
        if (mode) begin
            sel = 1'b1;
        end else begin
            sel = 1'b0;
        end
        @(posedge clk);
        sel = 1'b0;
    end

endmodule
