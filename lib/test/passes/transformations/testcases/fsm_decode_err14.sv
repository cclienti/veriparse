module sub_drv (input logic a, output logic y);
    assign y = ~a;
endmodule

module fsm_decode_err14 (input logic clk, input logic rst_n, input logic a,
                         output logic x);
    logic w;
    sub_drv u0 (.a(a), .y(w));
    (* veriparse_fsm *)
    initial begin
        x = 1'b0;
        @(posedge clk);
        x = w;
        @(posedge clk);
        x = 1'b0;
    end
endmodule
