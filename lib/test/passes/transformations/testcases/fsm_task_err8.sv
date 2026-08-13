module fsm_task_err8 (input logic clk, input logic rst_n,
                      input logic [7:0] a, output logic [7:0] q);
    task carry();
        begin
            logic [7:0] t;
            t = a;
            @(posedge clk);
            q <= t;
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        carry();
        @(posedge clk);
    end
endmodule
