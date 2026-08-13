module fsm_task7 (
    input logic clk,
    input logic rst_n,
    output logic q
);

    task pulse;
        begin
            q <= 1'b1;
        end
    endtask

    (* veriparse_fsm *)
    initial begin
        q <= 1'b0;
        @(posedge clk);
        pulse();
        @(posedge clk);
    end

endmodule
