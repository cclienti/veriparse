module fsm_temp_err4 (
    input logic clk,
    input logic rst_n,
    input logic c,
    input logic [7:0] a,
    output logic [7:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        begin
            logic [7:0] t;
            t = a;
            if (c) t = a + 8'd1;
            q <= t;
        end
        @(posedge clk);
    end

endmodule
