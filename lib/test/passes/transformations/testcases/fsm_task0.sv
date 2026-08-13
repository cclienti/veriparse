module fsm_task0 (input logic clk, input logic rst_n,
                  output logic scl, output logic done);
    task pulse();
        scl <= 1'b0;
        @(posedge clk);
        scl <= 1'b1;
        @(posedge clk);
    endtask
    (* veriparse_fsm *)
    initial begin
        scl <= 1'b1; done <= 1'b0;
        @(posedge clk);
        pulse();
        pulse();
        done <= 1'b1;
    end
endmodule
