module fsm_task3 (input logic clk, input logic rst_n,
                  output logic scl, output logic done);
    task half(input logic v);
        begin
            scl <= v;
            @(posedge clk);
        end
    endtask
    task byte_bit();
        begin
            half(1'b0);
            half(1'b1);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        scl <= 1'b1; done <= 1'b0;
        @(posedge clk);
        byte_bit();
        byte_bit();
        done <= 1'b1;
    end
endmodule
