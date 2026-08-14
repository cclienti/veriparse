package seqlib0;
    task add_step(inout logic [7:0] acc, input logic [7:0] n);
        begin
            acc = acc + n;
        end
    endtask
endpackage

module fsm_pkg0 import seqlib0::*; (input logic clk, input logic rst_n,
                                    output logic [7:0] q, output logic done);
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        add_step(q, 8'd3);
        @(posedge clk);
        add_step(q, 8'd5);
        done <= 1'b1;
    end
endmodule
