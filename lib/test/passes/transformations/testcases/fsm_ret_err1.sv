module fsm_ret_err1 (input logic clk, input logic rst_n,
                     output logic [7:0] q, output logic done);
    task grab();
        begin
            q <= 8'd1;
            @(posedge clk);
            return 8'd5;
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        grab();
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
