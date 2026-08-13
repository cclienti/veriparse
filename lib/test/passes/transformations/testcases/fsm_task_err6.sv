module fsm_task_err6 (input logic clk, input logic rst_n,
                      output logic [7:0] q);
    task step(input logic [7:0] n);
        begin
            n = n + 8'd1;
            q <= n;
            @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        step(8'd3);
        @(posedge clk);
    end
endmodule
