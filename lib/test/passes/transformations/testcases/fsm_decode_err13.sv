module fsm_decode_err13 (input logic clk, input logic rst_n, input logic c,
                         output logic sel);
    logic c_r;
    (* veriparse_fsm *)
    initial begin
        c_r <= 1'b0;
        sel = 1'b0;
        @(posedge clk);
        c_r <= c;
        sel = 1'b0;
        @(posedge clk);
        if (c_r) begin
            logic sel;
            sel = 1'b0;
        end else begin
            sel = 1'b1;
        end
        @(posedge clk);
        sel = 1'b0;
    end
endmodule
