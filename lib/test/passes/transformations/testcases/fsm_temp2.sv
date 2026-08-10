module fsm_temp2 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] a,
    input logic [7:0] b,
    output logic [7:0] q,
    output logic [7:0] r,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        r <= '0;
        done <= 1'b0;
        @(posedge clk);
        begin
            logic [7:0] t;
            t = a & b;
            q <= t;
        end
        @(posedge clk);
        begin
            logic [7:0] t;
            t = a & b;
            r <= t;
        end
        @(posedge clk);
        done <= 1'b1;
    end

endmodule
