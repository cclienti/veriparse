module drv_old (clk, out);
    input clk;
    output [7:0] out;
    reg [7:0] out;

    always @(posedge clk) out <= 8'd3;
endmodule

module fsm_multidrive_err4 (
    input logic clk,
    input logic rst_n,
    output logic [7:0] q
);

    drv_old u_child (.clk(clk), .out(q));

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        q <= 8'd1;
        @(posedge clk);
    end

endmodule
