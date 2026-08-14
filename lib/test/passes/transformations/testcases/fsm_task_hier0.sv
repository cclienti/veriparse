module fsm_task_hier0_sub ();
    logic [7:0] v;
endmodule

module fsm_task_hier0 (input logic clk, input logic rst_n, input logic [7:0] d,
                       output logic [7:0] r, output logic done);
    fsm_task_hier0_sub u1 ();
    task mix(input logic [7:0] v);
        begin
            r <= u1.v ^ v;
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        r <= '0; done <= 1'b0;
        @(posedge clk);
        mix(d);
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
