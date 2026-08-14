module fsm_funcref_err0 (input logic clk, input logic rst_n,
                         input logic [7:0] d, output logic [7:0] q);
    logic [7:0] acc;
    function automatic logic bump(ref logic [7:0] v);
        v = v + 8'd1;
        bump = v[0];
    endfunction
    (* veriparse_fsm *)
    initial begin
        q <= '0; acc <= 8'd0;
        @(posedge clk);
        q <= {7'd0, bump(acc)};
        @(posedge clk);
    end
endmodule
