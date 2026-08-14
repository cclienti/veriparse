module fsm_task_err28 (input logic clk, input logic rst_n,
                       output logic [7:0] q, output logic done);
    task step();
        begin
            logic [7:0] acc = 8'd0;
            acc <= acc + 8'd1;
            @(posedge clk);
            q <= acc;
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        step();
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
