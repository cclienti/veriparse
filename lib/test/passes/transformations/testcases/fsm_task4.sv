module fsm_task4 (input logic clk, input logic rst_n,
                  input logic [7:0] a, input logic [7:0] b,
                  output logic [7:0] q, output logic busy);
    task add_step();
        begin
            begin
                logic [8:0] s;
                s = a + b;
                q <= s[7:0];
            end
            busy = 1'b1;
            @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0;
        busy = 1'b0;
        @(posedge clk);
        add_step();
        busy = 1'b0;
        q <= q + 8'd1;
        @(posedge clk);
        busy = 1'b0;
    end
endmodule
