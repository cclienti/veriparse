module fsm_task12 (input logic clk, input logic rst_n,
                   output logic [7:0] q, output logic done);
    task bump(input logic [7:0] f);
        begin
            f <= f + 8'd1;
            @(posedge clk);
            q <= f;
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        bump(8'd5);
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
