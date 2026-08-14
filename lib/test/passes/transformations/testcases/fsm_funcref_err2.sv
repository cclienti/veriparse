module fsm_funcref_err2 (input logic clk, input logic rst_n,
                         input logic [7:0] w, output logic [7:0] q,
                         output logic done);
    function automatic logic pick(const ref logic b);
        begin
            pick = b;
        end
    endfunction
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        q <= {7'd0, pick(w[0])};
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
