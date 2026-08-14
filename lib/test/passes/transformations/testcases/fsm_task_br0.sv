module fsm_task_br0 (input logic clk, input logic rst_n, input logic go,
                     input logic [7:0] d, output logic [7:0] r, output logic done);
    task helper(input logic [7:0] v);
        begin
            r <= v + 8'd1;
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        r <= '0; done <= 1'b0;
        @(posedge clk);
        if (go) helper(d);
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
