interface fsm_iface2_bus;
    logic [7:0] data;
    logic       valid;
    modport dev(input data, input valid);
endinterface

// A ref formal aliasing an interface member — a member is a variable, so the
// §13.5.2 net refusal cannot apply — beside a temporary whose name is the
// member's leaf name: the two never alias (ADR-0014 §7.4, §6.1).
module fsm_iface2(input var logic clk, input logic rst_n, fsm_iface2_bus.dev bus,
                  output logic [7:0] q);
    task automatic mix(ref logic [7:0] d, input logic [7:0] k);
        begin
            @(posedge clk);
            q <= d ^ k;
        end
    endtask

    (* veriparse_fsm *)
    initial begin
        q <= 8'h00;
        forever begin
            @(posedge clk);
            begin
                logic [7:0] data;
                data = bus.data + 8'd1;
                q <= data;
            end
            mix(bus.data, 8'h5a);
        end
    end
endmodule
