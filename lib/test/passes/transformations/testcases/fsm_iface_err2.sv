interface fsm_iface_err2_bus;
    logic [7:0] data;
    modport dev(input data);
endinterface

module fsm_iface_err2_sub(input logic c);
    logic [7:0] r;
endmodule

// A hierarchical ref actual that is not an interface member: substitution
// would alias storage of another scope (ADR-0014 §7.4, IEEE 1800-2017
// §13.5.2).
module fsm_iface_err2(input var logic clk, input logic rst_n, fsm_iface_err2_bus.dev bus,
                      output logic [7:0] q);
    fsm_iface_err2_sub u(.c(clk));
    task automatic mix(ref logic [7:0] d);
        begin
            @(posedge clk);
            q <= d;
        end
    endtask

    (* veriparse_fsm *)
    initial begin
        q <= 8'h00;
        forever begin
            @(posedge clk);
            mix(u.r);
        end
    end
endmodule
