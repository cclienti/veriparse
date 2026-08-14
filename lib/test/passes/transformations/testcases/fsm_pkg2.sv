package seqlib2;
    task automatic hold_n(const ref logic ck, input logic [7:0] n);
        begin
            (* veriparse_no_unroll *)
            repeat (n) @(posedge ck);
        end
    endtask
endpackage

module fsm_pkg2 import seqlib2::*; (input var logic clk, input logic rst_n,
                                    input logic [7:0] t_hold,
                                    output logic [7:0] q, output logic done);
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        q <= 8'd1;
        @(posedge clk);
        hold_n(clk, t_hold);
        q <= 8'd2;
        hold_n(clk, 8'd2);
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
