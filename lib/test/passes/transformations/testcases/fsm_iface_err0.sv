interface fsm_iface_err0_bus;
    logic req;
    logic ack;
    modport dev(input req, output ack);
endinterface

// '=' to an interface member: a member of an interface port is storage of
// the machine, so it takes '<=' like any other register (ADR-0014 §6.2).
module fsm_iface_err0(input var logic clk, input logic rst_n, fsm_iface_err0_bus.dev bus);
    (* veriparse_fsm *)
    initial begin
        bus.ack <= 1'b0;
        forever begin
            @(posedge clk);
            bus.ack = bus.req;
        end
    end
endmodule
