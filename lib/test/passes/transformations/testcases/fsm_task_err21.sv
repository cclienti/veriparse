module fsm_task_err21 (input logic clk, input logic rst_n, input logic [7:0] d,
                       output logic [7:0] q, output logic done);
    logic [7:0] grab_held;
    task grab();
        begin
            logic [7:0] held;
            held <= d;
            @(posedge clk);
            q <= held;
            @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        grab();
        done <= 1'b1;
    end
endmodule
