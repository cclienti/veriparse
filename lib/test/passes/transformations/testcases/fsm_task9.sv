module fsm_task9 (input logic clk, input logic rst_n,
                  input logic [7:0] d, output logic [7:0] q,
                  output logic done);
    task step();
        begin
            logic [7:0] tmp;
            tmp <= d;
            @(posedge clk);
            q <= q + tmp;
            @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        step();
        step();
        done <= 1'b1;
    end
endmodule
