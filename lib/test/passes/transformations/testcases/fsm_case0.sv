module fsm_case0 (
    input logic clk,
    input logic rst_n,
    input logic [1:0] sel,
    input logic [7:0] d,
    output logic [7:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        case (sel)
            2'd0: begin
                q <= d;
                @(posedge clk);
            end
            2'd1, 2'd2: begin
                q <= ~d;
                @(posedge clk);
                q <= q + 8'd1;
                @(posedge clk);
            end
        endcase
        q <= q << 1;
        @(posedge clk);
    end

endmodule
