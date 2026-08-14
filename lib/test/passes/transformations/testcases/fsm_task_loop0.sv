module fsm_task_loop0 (input logic clk, input logic rst_n, input logic [7:0] d,
                       output logic [7:0] q, output logic done);
    integer i;
    task put(input logic [7:0] v);
        begin
            q <= v;
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        for (i = 0; i < 2; i = i + 1) begin
            put(d + i);
            @(posedge clk);
        end
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
