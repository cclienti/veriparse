module fsm_task2 (input logic clk, input logic rst_n,
                  output logic [7:0] q, output logic done);
    task bump(input logic [7:0] n);
        begin
            repeat (n) begin
                q <= q + 8'd1;
                @(posedge clk);
            end
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        bump(8'd3);
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
