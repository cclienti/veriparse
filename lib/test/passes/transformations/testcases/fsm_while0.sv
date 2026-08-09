module fsm_while0 (
    input logic clk,
    input logic rst_n,
    input logic start,
    input logic [7:0] d,
    output logic [7:0] q,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        done <= 1'b0;
        @(posedge clk);
        while (!start) @(posedge clk);
        q <= d;
        @(posedge clk);
        done <= 1'b1;
    end

endmodule
