module fsm_task_err13 (input logic clk, input logic rst_n,
                       output logic [7:0] r);
    task automatic wrong(const ref logic [7:0] x);
        begin
            x = 8'd1;
            @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        r <= '0;
        @(posedge clk);
        wrong(r);
        @(posedge clk);
    end
endmodule
