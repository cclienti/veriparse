module fsm_hint2 (
    input logic clk,
    input logic clear,
    input logic [7:0] d,
    output logic [7:0] q,
    output logic done
);

    (* veriparse_fsm, veriparse_reset = "clear", veriparse_reset_level = 0,
       veriparse_reset_kind = "async", veriparse_prefix = "seq" *)
    initial begin
        q <= '0;
        done <= 1'b0;
        @(posedge clk);
        q <= d;
        @(posedge clk);
        done <= 1'b1;
    end

endmodule
