module fsm_temp9 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] a,
    output logic [7:0] q,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        done <= 1'b0;
        @(posedge clk);
        begin
            logic [3:0] t;
            t = 8'd200;
            q <= {4'b0, t};
        end
        @(posedge clk);
        done <= 1'b1;
    end

endmodule
