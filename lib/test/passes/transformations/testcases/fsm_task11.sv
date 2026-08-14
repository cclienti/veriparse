module fsm_task11 (input var logic clk, input logic rst_n, input var logic en,
                   output logic [7:0] q, output logic done);
    task automatic step(const ref logic ck, const ref logic ce);
        begin
            q <= q + 8'd1;
            @(posedge ck iff ce);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk iff en);
        step(clk, en);
        step(clk, en);
        done <= 1'b1;
    end
endmodule
