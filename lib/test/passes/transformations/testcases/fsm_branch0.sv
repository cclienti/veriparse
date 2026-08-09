module fsm_branch0 (
    input logic clk,
    input logic rst_n,
    input logic mode,
    input logic [7:0] d,
    output logic [7:0] acc,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        acc <= '0;
        done <= 1'b0;
        @(posedge clk);
        if (mode) begin
            acc <= d;
            @(posedge clk);
            acc <= acc + 8'd1;
            @(posedge clk);
        end
        else begin
            acc <= 8'hff - d;
            @(posedge clk);
        end
        if (acc[0]) acc <= acc ^ 8'h55;
        @(posedge clk);
        done <= 1'b1;
    end

endmodule
