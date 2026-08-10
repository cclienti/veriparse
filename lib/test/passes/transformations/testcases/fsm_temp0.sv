module fsm_temp0 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] a,
    input logic [7:0] b,
    output logic [7:0] q,
    output logic carry,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        carry <= 1'b0;
        done <= 1'b0;
        @(posedge clk);
        begin
            logic [8:0] sum;
            sum = a + b;
            q <= sum[7:0];
            carry <= sum[8];
        end
        @(posedge clk);
        done <= 1'b1;
    end

endmodule
