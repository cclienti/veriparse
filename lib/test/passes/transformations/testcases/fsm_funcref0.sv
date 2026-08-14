module fsm_funcref0 (input logic clk, input logic rst_n,
                     input logic [7:0] d, output logic [7:0] q,
                     output logic p, output logic done);
    logic [7:0] acc;
    function automatic logic parity(const ref logic [7:0] v);
        parity = ^v;
    endfunction
    (* veriparse_fsm *)
    initial begin
        q <= '0; acc <= 8'd0; p <= 1'b0; done <= 1'b0;
        @(posedge clk);
        acc <= d;
        @(posedge clk);
        p <= parity(acc);
        q <= acc;
        @(posedge clk);
        done <= 1'b1;
    end
endmodule
