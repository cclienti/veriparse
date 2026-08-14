module fsm_task_err15 (input var logic clk, input logic rst_n,
                       input var logic en, input var logic en2,
                       output logic [7:0] q);
    task automatic step(const ref logic ck, const ref logic ce);
        begin
            q <= q + 8'd1;
            @(posedge ck iff ce);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk iff en);
        step(clk, en);
        step(clk, en2);
    end
endmodule
