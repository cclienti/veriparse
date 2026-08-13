module fsm_task_err11 (input logic clk, input logic rst_n,
                       input logic [7:0] d, output logic [7:0] q);
    task automatic bad();
        begin
            logic [7:0] t;
            q <= t;
            t <= d;
            @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        bad();
        @(posedge clk);
    end
endmodule
