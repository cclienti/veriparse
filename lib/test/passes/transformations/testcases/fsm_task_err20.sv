module fsm_task_err20 (input logic clk, input logic rst_n,
                       output s, output logic done);
    task automatic blip(ref logic x);
        begin
            x = 1'b1;
            @(posedge clk);
            x = 1'b0;
            @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        s = 1'b0; done <= 1'b0;
        @(posedge clk);
        blip(s);
        done <= 1'b1;
    end
endmodule
