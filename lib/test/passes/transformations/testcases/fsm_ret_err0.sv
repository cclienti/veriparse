module fsm_ret_err0 (input logic clk, input logic rst_n, input logic stop,
                     output logic [7:0] q, output logic done);
    task spin();
        begin
            (* veriparse_no_unroll *)
            repeat (8'd4) begin
                if (stop) return;
                q <= q + 8'd1;
                @(posedge clk);
            end
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        spin();
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
