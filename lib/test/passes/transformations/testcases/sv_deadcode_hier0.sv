interface sv_deadcode_hier0_bus;
    logic [7:0] sum;
    modport dev(output sum);
endinterface

// A write through a hierarchical path leaves this module: its leaf name
// says nothing about the local signal of the same name, so liveness here
// can neither justify removing it nor let the local one keep it alive.
// The local `sum` is genuinely dead and goes; `bus.sum` stays.
module sv_deadcode_hier0(input logic clk, sv_deadcode_hier0_bus.dev bus);
    logic [7:0] sum;

    always_ff @(posedge clk) begin
        bus.sum <= 8'd7;
        sum <= 8'd9;
    end
endmodule
