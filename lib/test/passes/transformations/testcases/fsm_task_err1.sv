module fsm_task_err1 (input logic clk, input logic rst_n, output logic y);
    task spin();
        begin
            y <= 1'b0;
            @(posedge clk);
            spin();
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        y <= 1'b1;
        @(posedge clk);
        spin();
    end
endmodule
