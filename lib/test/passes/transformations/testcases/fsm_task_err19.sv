module fsm_task_err19 (input logic clk, input logic rst_n,
                       output logic [7:0] q, output logic done);
    task automatic step(const ref logic ck);
        begin
            q <= q + 8'd1;
            @(posedge ck);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        step(clk);
        done <= 1'b1;
    end
endmodule
