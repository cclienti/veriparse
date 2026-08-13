module fsm_task1 (input logic clk, input logic rst_n,
                  input logic [7:0] t_low, input logic [7:0] t_high,
                  output logic scl, output logic done);
    task phase(input logic v, input logic [7:0] n);
        begin
            scl <= v;
            (* veriparse_no_unroll *)
            repeat (n) @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        scl <= 1'b1; done <= 1'b0;
        @(posedge clk);
        phase(1'b0, t_low);
        phase(1'b1, t_high);
        phase(1'b0, 8'd2);
        phase(1'b1, 8'd2);
        done <= 1'b1;
    end
endmodule
