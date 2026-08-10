module fsm_multidrive0 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] d,
    output logic [7:0] q,
    output logic [7:0] y,
    output logic done
);

    always_comb begin : b
        logic [7:0] q;
        q = d ^ 8'h55;
        y = q;
    end

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        done <= 1'b0;
        @(posedge clk);
        q <= d;
        @(posedge clk);
        done <= 1'b1;
    end

endmodule
