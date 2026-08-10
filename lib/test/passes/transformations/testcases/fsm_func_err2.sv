module fsm_func_err2 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] d,
    output logic [7:0] q
);

    function logic [7:0] stamp(input logic [7:0] x);
        logic [7:0] n;
        begin
            n = n + 8'd1;
            stamp = x + n;
        end
    endfunction

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        q <= stamp(d);
        @(posedge clk);
    end

endmodule
