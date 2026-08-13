module fsm_task_err12 (input logic clk, input logic rst_n,
                       input logic [7:0] d, output logic [7:0] q,
                       output logic [7:0] r);
    task res(output logic [7:0] o);
        begin
            q <= o;
            o <= d;
            @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; r <= '0;
        @(posedge clk);
        res(r);
        @(posedge clk);
    end
endmodule
