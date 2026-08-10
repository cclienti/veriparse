module fsm_multidrive1 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] d,
    output logic [1:0] idx,
    output logic [15:0] wide,
    output logic done
);

    always_ff @(posedge clk)
        wide[idx * 4 +: 4] <= d[3:0];

    (* veriparse_fsm *)
    initial begin
        idx <= '0;
        done <= 1'b0;
        @(posedge clk);
        idx <= 2'd2;
        @(posedge clk);
        done <= 1'b1;
    end

endmodule
