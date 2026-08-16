module fsm_iface_err1_sub(input logic c);
    logic q;
endmodule

// '<=' through a hierarchical path that is not an interface port of this
// module: no machine here owns that storage (ADR-0014 §6).
module fsm_iface_err1(input var logic clk, input logic rst_n);
    fsm_iface_err1_sub u(.c(clk));
    (* veriparse_fsm *)
    initial begin
        forever begin
            @(posedge clk);
            u.q <= 1'b1;
        end
    end
endmodule
