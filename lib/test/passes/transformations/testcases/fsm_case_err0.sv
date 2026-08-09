module fsm_case_err0 (
    input logic clk,
    input logic rst_n,
    input logic [1:0] sel,
    output logic q
);

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
        @(posedge clk);
        casez (sel)
            2'b1?: begin
                q <= 1'b1;
                @(posedge clk);
            end
            default: q <= 1'b0;
        endcase
        @(posedge clk);
    end

endmodule
