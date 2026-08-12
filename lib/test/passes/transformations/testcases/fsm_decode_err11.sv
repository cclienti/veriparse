module fsm_decode_err11 (input logic clk, input logic rst_n,
                         output logic [7:0] q);
    localparam LIMIT = 3;
    (* veriparse_fsm *)
    initial begin
        q <= '0;
        @(posedge clk);
        LIMIT = 4;
        q <= LIMIT;
        @(posedge clk);
    end
endmodule
