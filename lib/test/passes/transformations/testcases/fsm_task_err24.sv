module fsm_task_err24 (input logic clk, input logic rst_n,
                       output logic [7:0] q, output logic done);
    logic [7:0] flag;
    task probe(output logic [7:0] o);
        begin
            if (o[0]) q <= q + 8'd1;
            o <= q;
            @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        probe(flag);
        probe(flag);
        done <= 1'b1;
    end
endmodule
