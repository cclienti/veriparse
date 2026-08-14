module fsm_task_err22 (input var logic clk, input logic rst_n,
                       output logic [7:0] q, output logic done);
    task automatic step(const ref logic c);
        begin
            q <= 8'd1;
            @(posedge c);
            begin
                logic [7:0] c;
                c = 8'd2;
                q <= c;
            end
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        step(clk);
        done <= 1'b1;
    end
endmodule
