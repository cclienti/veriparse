module fsm_task_err26 (input var logic clk, input logic rst_n,
                       output logic [7:0] q, output logic done);
    task inner(output logic [7:0] o);
        begin
            o = 8'd5;
        end
    endtask
    task automatic step(const ref logic [7:0] c);
        begin
            inner(c);
            q <= c;
        end
    endtask
    logic [7:0] cfg;
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0; cfg <= 8'd1;
        @(posedge clk);
        step(cfg);
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
