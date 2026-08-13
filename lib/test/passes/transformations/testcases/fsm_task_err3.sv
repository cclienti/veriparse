module fsm_task_err3 (input logic clk, input logic rst_n,
                      output logic [7:0] r);
    task grab(output logic [7:0] o);
        begin
            o = 8'd1;
            @(posedge clk);
            o = 8'd2;
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        r <= '0;
        @(posedge clk);
        grab(r);
        @(posedge clk);
    end
endmodule
