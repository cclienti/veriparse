interface fsm_iface0_bus;
    logic       req;
    logic       ack;
    logic [7:0] data;
    modport dev(input req, input data, output ack);
endinterface

// A machine driving a member of an interface port: the interface is kept as
// it stands and its members are signals of the machine like any other
// (ADR-0014 §6, IEEE 1800-2017 §25.3).
module fsm_iface0(input var logic clk, input logic rst_n, fsm_iface0_bus.dev bus,
                  output logic [7:0] q);
    (* veriparse_fsm *)
    initial begin
        bus.ack <= 1'b0;
        q <= 8'h00;
        forever begin
            @(posedge clk);
            if(bus.req) begin
                bus.ack <= 1'b1;
                q <= bus.data;
                @(posedge clk);
                bus.ack <= 1'b0;
            end
        end
    end
endmodule
