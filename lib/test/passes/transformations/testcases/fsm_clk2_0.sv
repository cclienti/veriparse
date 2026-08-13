module fsm_clk2_0 (input logic clk, input logic clk2, input logic rst_n,
                   input logic [7:0] d, output logic [7:0] q,
                   output logic [7:0] r2, output logic done);
    always_ff @(posedge clk2) r2 <= d;

    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        q <= 8'd1;
        @(posedge clk);
        q <= q + 8'd2;
        @(posedge clk);
        q <= q ^ 8'h55;
        @(posedge clk);
        done <= 1'b1;
    end
endmodule
