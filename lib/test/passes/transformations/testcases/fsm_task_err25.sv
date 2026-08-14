module fsm_task_err25 (input logic clk, input logic rst_n,
                       output logic [7:0] q, output logic done);
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        undeclared_task(q);
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
