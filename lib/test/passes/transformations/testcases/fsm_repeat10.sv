module fsm_repeat10 (input logic clk, input logic rst_n,
                     input logic stop, input logic [7:0] n,
                     output logic [7:0] q, output logic done);
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        (* veriparse_no_unroll *)
        while (!stop) begin
            (* veriparse_no_unroll *)
            repeat (n) @(posedge clk);
            q <= q + 8'd1;
            @(posedge clk);
        end
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
