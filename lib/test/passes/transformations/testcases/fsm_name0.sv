module fsm_name0 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] d,
    output logic [7:0] q,
    output logic [7:0] r,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        begin : SETUP
            q <= '0;
            r <= '0;
            done <= 1'b0;
        end
        @(posedge clk);
        begin : IDLE
            r <= d;
        end
        begin : COUNT
            q <= q + 8'd1;
            @(posedge clk);
            q <= q + 8'd2;
            @(posedge clk);
        end
        begin : BIT
            begin : LOW
                q <= q ^ 8'h0f;
                @(posedge clk);
            end
            begin : HIGH
                q <= q ^ 8'hf0;
                @(posedge clk);
            end
        end
        done <= 1'b1;
        @(posedge clk);
    end

endmodule
