module fsm_task_err14 (input logic clk, input logic clk2, input logic rst_n,
                       output logic [7:0] q);
    task automatic tick(const ref logic ck);
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
        tick(clk2);
    end
endmodule
