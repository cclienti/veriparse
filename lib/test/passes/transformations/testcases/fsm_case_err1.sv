module fsm_case_err1 (
    input logic clk,
    input logic rst_n,
    input logic [1:0] sel,
    output logic q
);

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
        @(posedge clk);
        case (sel)
            default: begin
                q <= 1'b1;
                @(posedge clk);
            end
            default: begin
                q <= 1'b0;
                @(posedge clk);
            end
        endcase
        @(posedge clk);
    end

endmodule
