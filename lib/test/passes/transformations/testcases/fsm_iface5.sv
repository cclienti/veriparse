interface fsm_iface5_bus;
    logic [7:0] d;
    logic [7:0] acc;
    modport dev(input d, output acc);
endinterface

// A constant index keeps a static identity, so the elements of an
// interface-port array are distinct signals of the machine and each keeps
// its own storage (ADR-0014 §6.3).
module fsm_iface5(input var logic clk, input logic rst_n, fsm_iface5_bus.dev bus[2],
                  output logic [7:0] q);
    (* veriparse_fsm *)
    initial begin
        bus[0].acc <= 8'h00;
        bus[1].acc <= 8'h00;
        q <= 8'h00;
        @(posedge clk);
        forever begin
            @(posedge clk);
            bus[0].acc <= bus[0].d;
            bus[1].acc <= bus[1].d;
            q <= bus[0].d + bus[1].d;
        end
    end
endmodule
