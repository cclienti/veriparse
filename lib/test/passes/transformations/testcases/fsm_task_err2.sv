module fsm_task_err2 (input logic clk, input logic rst_n, output logic y);
    task a_step();
        begin
            y <= 1'b0;
            @(posedge clk);
            b_step();
        end
    endtask
    task b_step();
        begin
            y <= 1'b1;
            @(posedge clk);
            a_step();
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        y <= 1'b1;
        @(posedge clk);
        a_step();
    end
endmodule
