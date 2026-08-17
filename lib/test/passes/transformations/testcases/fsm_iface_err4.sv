interface fsm_iface_err4_bus;
    logic [7:0] d;
    modport dev(input d);
endinterface

// A hierarchical name indexed by a variable has no static identity: two
// such names cannot be told apart, so definedness and commit ordering
// would both be decided on a collapsed key (ADR-0014 §6.3, §9).
module fsm_iface_err4(input var logic clk, input logic rst_n, fsm_iface_err4_bus.dev bus[2],
                      input logic [0:0] i, output logic [7:0] q);
    (* veriparse_fsm *)
    initial begin
        q <= 8'h00;
        @(posedge clk);
        forever begin
            @(posedge clk);
            q <= bus[i].d;
        end
    end
endmodule
