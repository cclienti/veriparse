module fsm_task_err16 (input logic clk, input logic rst_n,
                       output logic [7:0] q);
    task automatic tick(input logic ck);
        begin
            q <= q + 8'd1;
            @(posedge ck);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        tick(clk);
    end
endmodule
