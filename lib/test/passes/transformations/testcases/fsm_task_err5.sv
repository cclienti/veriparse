module fsm_task_err5 (input logic clk, input logic rst_n,
                      output logic [7:0] r);
    task automatic use_ref(ref logic [7:0] x);
        x <= 8'd1;
        @(posedge clk);
    endtask
    (* veriparse_fsm *)
    initial begin
        r <= '0;
        @(posedge clk);
        use_ref(r);
        @(posedge clk);
    end
endmodule
