module fsm_task_err29 (input logic clk, input logic rst_n, input logic flag,
                       output logic [7:0] q, output logic done);
    logic [7:0] got;
    task automatic get(output logic [7:0] o);
        begin
            if (flag) o = 8'd1;
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        get(got);
        q <= got;
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
