module fsm_branch_err3 (
    input logic clk,
    input logic rst_n,
    input logic mode,
    output logic [7:0] acc
);

    (* veriparse_fsm *)
    initial begin
        if (mode) acc <= 8'd1;
        else acc <= 8'd2;
        @(posedge clk);
        acc <= acc + 8'd1;
        @(posedge clk);
    end

endmodule
