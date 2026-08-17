interface fsm_iface4_bus;
    logic [7:0] sum;
    modport dev(output sum);
endinterface

// §13.3 copy-out visibility is immediate, and an interface member is no
// exception: the '='-written output formal is a §6.1 value, and the read
// after the call sees what the call left rather than the member's entry
// value (ADR-0014 §6.1, §6.3, §7.4).
module fsm_iface4(input var logic clk, input logic rst_n, fsm_iface4_bus.dev bus,
                  output logic [7:0] q, output logic [7:0] s);
    task set90(output logic [7:0] v);
        begin
            v = 8'd90;
        end
    endtask

    (* veriparse_fsm *)
    initial begin
        bus.sum <= 8'h00;
        q <= 8'h00;
        s <= 8'h00;
        @(posedge clk);
        forever begin
            @(posedge clk);
            set90(s);
            q <= s;
            @(posedge clk);
            set90(bus.sum);
            q <= bus.sum;
        end
    end
endmodule
