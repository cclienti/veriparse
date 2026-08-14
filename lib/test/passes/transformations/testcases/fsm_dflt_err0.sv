module fsm_dflt_err0 (input var logic clk, input logic rst_n,
                      output logic [7:0] q, output logic done);
    task automatic hold(const ref logic ck, input logic [7:0] n);
        begin
            (* veriparse_no_unroll *)
            repeat (n) @(posedge ck);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        hold(clk);
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
