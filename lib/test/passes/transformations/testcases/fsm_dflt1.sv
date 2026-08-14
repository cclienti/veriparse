package dfltlib;
    localparam logic [7:0] T_HOLD = 8'd3;
    task automatic hold(const ref logic ck, input logic [7:0] n = T_HOLD);
        begin
            (* veriparse_no_unroll *)
            repeat (n) @(posedge ck);
        end
    endtask
endpackage

module fsm_dflt1 import dfltlib::*; (input var logic clk, input logic rst_n,
                                     output logic [7:0] q, output logic done);
    (* veriparse_fsm *)
    initial begin
        q <= '0; done <= 1'b0;
        @(posedge clk);
        q <= 8'd1;
        hold(clk);
        done <= 1'b1;
        @(posedge clk);
    end
endmodule
