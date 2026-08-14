module fsm_dflt0 (input var logic clk, input logic rst_n,
                  output logic [7:0] q, output logic done);
    task automatic hold(const ref logic ck, input logic [7:0] n = 8'd2);
        begin
            (* veriparse_no_unroll *)
            repeat (n) @(posedge ck);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        q <= 8'd1;
        hold(clk);
        q <= 8'd2;
        hold(clk, 8'd4);
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
