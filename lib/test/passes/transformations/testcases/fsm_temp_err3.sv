module fsm_temp_err3 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] a,
    output logic [7:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        begin
            logic [7:0] t;
            q <= t + a;
            t = a;
        end
        @(posedge clk);
    end

endmodule
