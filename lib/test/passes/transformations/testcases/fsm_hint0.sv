module fsm_hint0 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] d,
    output logic [7:0] q,
    output logic done
);

    (* veriparse_fsm, veriparse_encoding = "one_hot" *)
    initial begin
        q <= '0;
        done <= 1'b0;
        @(posedge clk);
        q <= d;
        @(posedge clk);
        q <= q + 8'd1;
        @(posedge clk);
        done <= 1'b1;
    end

endmodule
