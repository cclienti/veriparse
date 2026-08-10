module fsm_temp3 (
    input logic clk,
    input logic rst_n,
    input logic c,
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
        if (c) begin
            logic [7:0] u;
            u = a | b;
            q <= u;
        end
        @(posedge clk);
        done <= 1'b1;
    end

endmodule
