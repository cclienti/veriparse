module fsm_temp_err7 (
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
            @(posedge clk);
            q <= t + a;
            begin
                logic [7:0] t;
                t = a;
            end
        end
    end

endmodule
