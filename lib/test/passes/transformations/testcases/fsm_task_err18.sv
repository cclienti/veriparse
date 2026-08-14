module fsm_task_err18 (input logic clk, input logic rst_n,
                       output logic [7:0] q, output logic done);
    task automatic bump(input logic [7:0] n, inout logic [7:0] acc);
        begin
            repeat (n) begin
                acc <= acc + 8'd1;
                @(posedge clk);
            end
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
