module fsm_funcref_err1 (input logic clk, input logic rst_n,
                         input logic [7:0] d, output logic [7:0] q);
    logic [7:0] acc;
    function logic parity(const ref logic [7:0] v);
        parity = ^v;
    endfunction
    (* veriparse_fsm *)
    initial begin
        q <= '0; acc <= 8'd0;
        @(posedge clk);
        q <= {7'd0, parity(acc)};
        @(posedge clk);
    end
endmodule
