module fsm_task_err9 (input logic clk, input logic rst_n, output logic y);
    task slow();
        begin
            y <= 1'b0;
            #5;
            @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        y <= 1'b1;
        @(posedge clk);
        slow();
    end
endmodule
