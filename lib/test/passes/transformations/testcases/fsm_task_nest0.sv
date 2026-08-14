module fsm_task_nest0 (input logic clk, input logic rst_n, input logic [7:0] x,
                       output logic [7:0] r, output logic done);
    task inner(output logic [7:0] o);
        begin
            o = 8'd7;
        end
    endtask
    task automatic outer(input logic [7:0] f);
        begin
            inner(f);
            r <= f;
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        r <= '0; done <= 1'b0;
        @(posedge clk);
        outer(x);
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
