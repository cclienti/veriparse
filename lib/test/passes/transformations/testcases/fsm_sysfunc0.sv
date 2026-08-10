module fsm_sysfunc0 (
    input logic clk,
    input logic rst_n,
    input logic [7:0] d,
    output logic [3:0] q,
    output logic done
);

    (* veriparse_fsm *)
    initial begin
        q <= '0;
        done <= 1'b0;
        @(posedge clk);
        q <= $countones(d);
        @(posedge clk);
        done <= 1'b1;
    end

endmodule
