module fsm_prune0 (
    input logic clk,
    input logic rst_n,
    input logic go,
    input logic rdy,
    output logic busy,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        busy <= 1'b0;
        done <= 1'b0;
        @(posedge clk);
        if (go && rdy) begin
            busy <= 1'b1;
            while (go && rdy) @(posedge clk);
        end
        busy <= 1'b0;
        @(posedge clk);
        done <= 1'b1;
    end

endmodule
