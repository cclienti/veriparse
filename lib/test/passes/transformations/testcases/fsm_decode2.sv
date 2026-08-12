module fsm_decode2 (
    input logic clk,
    input logic rst_n,
    input logic start,
    output logic busy,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        busy = 1'b0;
        done = 1'b0;
        @(posedge clk);
        forever begin
            busy = 1'b0;
            done = 1'b0;
            begin : ARMED
                while (!start) begin
                    busy = 1'b0;
                    done = 1'b0;
                    @(posedge clk);
                end
            end
            busy = 1'b1;
            done = 1'b0;
            @(posedge clk);
            busy = 1'b1;
            done = 1'b0;
            @(posedge clk);
            busy = 1'b0;
            done = 1'b1;
            @(posedge clk);
        end
    end

endmodule
