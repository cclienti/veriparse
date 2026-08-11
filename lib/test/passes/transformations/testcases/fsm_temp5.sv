module fsm_temp5 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] a,
    input logic [7:0] b,
    output logic [7:0] q,
    output logic neg,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        neg <= 1'b0;
        done <= 1'b0;
        @(posedge clk);
        begin
            logic signed [8:0] d;
            d = a - b;
            q <= d >>> 1;
            neg <= d < 0;
        end
        @(posedge clk);
        done <= 1'b1;
    end

endmodule
