module fsm_task_loop1 (input logic clk, input logic rst_n, input logic [7:0] step,
                       output logic [7:0] q, output logic done);
    task inner(input logic [7:0] v);
        begin
            q <= q + v;
        end
    endtask
    task outer(input logic [7:0] n);
        begin
            repeat (n) begin
                inner(step);
                @(posedge clk);
            end
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        outer(8'd2);
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
