module fsm_func_err1 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] d,
    output logic [7:0] q
);

    function automatic logic [7:0] noisy(input logic [7:0] x);
        noisy = x ^ $random;
    endfunction

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        q <= noisy(d);
        @(posedge clk);
    end

endmodule
