module fsm_temp_err6 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] a,
    output logic [7:0] q
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        forever begin
            logic [7:0] t;
            @(posedge clk);
            q <= t + a;
            t = a;
        end
    end

endmodule
