module fsm_task_nest1 (input logic clk, input logic rst_n,
                       output logic [7:0] r, output logic done);
    task inner(output logic [7:0] o);
        begin
            o = 8'd5;
        end
    endtask
    task step(input logic [7:0] a);
        begin
            inner(a);
            r <= a;
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        r <= '0; done <= 1'b0;
        @(posedge clk);
        step(8'd3);
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
