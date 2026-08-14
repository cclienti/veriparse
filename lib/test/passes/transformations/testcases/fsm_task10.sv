module fsm_task10 (input logic clk, input logic rst_n,
                   input logic [7:0] cfg_in, output logic [7:0] q,
                   output logic done);
    logic [7:0] cfg;
    task automatic apply(const ref logic [7:0] c);
        begin
            q <= c;
            @(posedge clk);
            q <= q + c;
            @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; cfg <= 8'd0; done <= 1'b0;
        @(posedge clk);
        cfg <= cfg_in;
        @(posedge clk);
        apply(cfg);
        apply(cfg);
        done <= 1'b1;
    end
endmodule
