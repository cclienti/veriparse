module fsm_ret_err2 (input logic clk, input logic rst_n,
                     input logic a, input logic b,
                     output logic [7:0] q, output logic done);
    task pick();
        begin
            q <= 8'd1;
            @(posedge clk);
            if (a) begin
                if (b) return;
                q <= 8'd2;
            end
            q <= 8'd3;
            @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        pick();
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
