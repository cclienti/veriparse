module fsm_temp6 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] a,
    input logic [7:0] b,
    output logic [7:0] q,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        done <= 1'b0;
        @(posedge clk);
        begin
            int t;
            t = a * b;
            q <= t[7:0];
        end
        @(posedge clk);
        done <= 1'b1;
    end

endmodule
