package seqlib1;
    task automatic blip(const ref logic ck, ref logic s);
        begin
            s = 1'b1;
            @(posedge ck);
            s = 1'b0;
            @(posedge ck);
        end
    endtask
endpackage

module fsm_pkg1 (input logic clk, input logic rst_n,
                 output logic strobe, output logic done);
    (* veriparse_fsm *)
    initial begin
        strobe = 1'b0; done <= 1'b0;
        @(posedge clk);
        seqlib1::blip(clk, strobe);
        seqlib1::blip(clk, strobe);
        strobe = 1'b0;
        done <= 1'b1;
    end
endmodule
