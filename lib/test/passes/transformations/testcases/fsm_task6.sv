module fsm_task6 (input logic clk, input logic rst_n,
                  output logic [7:0] q, output logic done);
    task bump(input logic [7:0] n, inout [7:0] q);
        repeat (n) begin
            q <= q + 8'd1;
            @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        bump(8'd3, q);
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
