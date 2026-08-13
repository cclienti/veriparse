module fsm_task8 (input logic clk, input logic rst_n,
                  output logic [7:0] q, output logic strobe);
    task automatic blip(ref logic s);
        begin
            s = 1'b1;
            q <= q + 8'd1;
            @(posedge clk);
            s = 1'b0;
            q <= q + 8'd1;
            @(posedge clk);
        end
    endtask
    (* veriparse_fsm *)
    initial begin
        q <= '0;
        strobe = 1'b0;
        @(posedge clk);
        blip(strobe);
        blip(strobe);
        strobe = 1'b0;
    end
endmodule
