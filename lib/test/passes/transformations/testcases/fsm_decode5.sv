module fsm_decode5 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] a,
    input logic [7:0] b,
    output logic [7:0] y,
    output logic busy
);

    logic [7:0] tmp;

    always_comb begin
        logic [7:0] tmp;
        tmp = a + b;
        y = tmp;
    end

    (* veriparse_fsm *)
    initial begin
        tmp  <= '0;
        busy = 1'b0;
        @(posedge clk);
        tmp <= a;
        busy = 1'b0;
        @(posedge clk);
        busy = tmp[0];
        @(posedge clk);
        busy = 1'b0;
    end

endmodule
