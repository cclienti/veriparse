module fsm_alpha3 (input logic clk, input logic rst_n, input logic [7:0] d,
                       output logic [7:0] q, output logic done);
    task step();
        begin
            begin
                logic [7:0] cnt;
                cnt <= d;
                @(posedge clk);
                q <= cnt;
            end
            begin
                logic [7:0] cnt;
                cnt <= d + 8'd1;
                @(posedge clk);
                q <= q + cnt;
            end
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        step();
        done <= 1'b1;
    end
endmodule
