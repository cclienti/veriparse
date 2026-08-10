module fsm_func_err0 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] d,
    output logic [7:0] q
);

    logic [7:0] shadow;

    function automatic logic [7:0] taint(input logic [7:0] x);
        shadow = x;
        taint = x + 8'd1;
    endfunction

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        q <= taint(d);
        @(posedge clk);
    end

endmodule
