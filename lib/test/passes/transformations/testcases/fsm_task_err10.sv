module fsm_task_err10 (input logic clk, input logic rst_n,
                       output logic scl, output logic [7:0] q);
    task pulse();
        begin
            scl <= 1'b0;
            @(posedge clk);
            scl <= 1'b1;
            @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        scl <= 1'b1;
        @(posedge clk);
        pulse();
    end
    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        pulse();
        q <= 8'd1;
        @(posedge clk);
    end
endmodule
