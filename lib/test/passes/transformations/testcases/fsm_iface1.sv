interface fsm_iface1_bus;
    logic       req;
    logic       ack;
    logic [7:0] data;
    modport dev(input req, input data, output ack);
endinterface

// Interface members as task actuals: an input formal captures one, an output
// formal copies out to another, and the copy-out keeps the whole path
// (ADR-0014 §7.4, IEEE 1800-2017 §13.3, §25.3).
module fsm_iface1(input var logic clk, input logic rst_n, fsm_iface1_bus.dev bus,
                  output logic [7:0] q);
    task handshake(input logic go, output logic done);
        begin
            @(posedge clk);
            done <= go;
            @(posedge clk);
        end
    endtask

    (* veriparse_fsm *)
    initial begin
        bus.ack <= 1'b0;
        q <= 8'h00;
        forever begin
            @(posedge clk);
            handshake(bus.req, bus.ack);
            q <= bus.data;
        end
    end
endmodule
