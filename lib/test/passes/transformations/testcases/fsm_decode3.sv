module fsm_decode3 (
    input logic clk,
    input logic rst_n,
    input logic m_in,
    output logic [7:0] q,
    output logic x
);

    logic m;

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        m <= 1'b0;
        x = 1'b0;
        @(posedge clk);
        m <= m_in;
        x = 1'b0;
        @(posedge clk);
        case (m)
            1'b0: begin
                x = 1'b1;
                q <= 8'd1;
            end
            1'b0: begin
                x = 1'b0;
                q <= 8'd2;
            end
            default: begin
                forever begin
                    x = 1'b0;
                    @(posedge clk);
                end
            end
        endcase
        @(posedge clk);
        x = 1'b0;
    end

endmodule
