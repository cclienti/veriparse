module fsm_task_err4 (input logic clk, input logic rst_n,
                      output logic [7:0] r);
    task adjust(inout logic [7:0] io);
        begin
            io = io + 8'd1;
            @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        r <= '0;
        @(posedge clk);
        adjust(r);
        @(posedge clk);
    end
endmodule
