module fsm_name1 (
    input logic clk,
    input logic rst_n,
    input logic x,
    output logic [7:0] q,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        done <= 1'b0;
        @(posedge clk);
        if (x) begin : FAST
            q <= 8'd1;
        end
        else begin : SLOW
            q <= 8'd2;
            @(posedge clk);
        end
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
