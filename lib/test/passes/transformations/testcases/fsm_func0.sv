module fsm_func0 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] d,
    output logic [7:0] q,
    output logic done
);

    function automatic logic [7:0] gray(input logic [7:0] x);
        gray = x ^ (x >> 1);
    endfunction

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        done <= 1'b0;
        @(posedge clk);
        q <= gray(d);
        @(posedge clk);
        done <= 1'b1;
    end

endmodule
