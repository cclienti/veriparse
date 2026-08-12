module fsm_decode_err5 (input logic clk, input logic rst_n, input logic m,
                        output logic sel);
    (* veriparse_fsm *)
    initial begin
        sel = 1'b0;
        @(posedge clk);
        if (m) begin
            sel = 1'b1;
            @(posedge clk);
            sel = 1'b1;
        end else begin
            sel = 1'b0;
            @(posedge clk);
            sel = 1'b0;
        end
        @(posedge clk);
        sel = 1'b0;
    end
endmodule
