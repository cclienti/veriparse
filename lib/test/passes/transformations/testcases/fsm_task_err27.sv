module fsm_task_err27 (input logic clk, input logic rst_n,
                       input logic [7:0] d, output logic [7:0] q,
                       output logic done);
    task step();
        begin
            logic [7:0] arr [0:3];
            arr[0] <= d;
            @(posedge clk);
            q <= arr[0];
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
