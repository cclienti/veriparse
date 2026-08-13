module fsm_task_err7 (input logic clk, input logic rst_n,
                      input logic [7:0] n_in, output logic [7:0] q);
    task wait_n(input logic [7:0] n);
        begin
            repeat (n) begin
                q <= q + 8'd1;
                @(posedge clk);
            end
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        wait_n(n_in);
        @(posedge clk);
    end
endmodule
