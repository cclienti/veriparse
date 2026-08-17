interface fsm_iface_err3_bus;
    logic [7:0] d;
    modport dev(input d);
endinterface

// A decoded output whose value reads an interface member this process does
// not drive: the member changes on the arrival edge like the input it is,
// so the emitted always_comb would track a value the source held for the
// whole cycle (ADR-0014 §6.2, §6.3).
module fsm_iface_err3(input var logic clk, input logic rst_n, fsm_iface_err3_bus.dev bus,
                      output logic [7:0] y);
    (* veriparse_fsm *)
    initial begin
        y = 8'h00;
        @(posedge clk);
        y = bus.d;
        @(posedge clk);
        y = 8'h01;
    end
endmodule
