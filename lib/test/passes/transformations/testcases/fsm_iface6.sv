interface fsm_iface6_bus(input logic clk);
    logic       req;
    logic       ack;
    logic [7:0] data;

    // The handshake helper living with the bus it drives (IEEE 1800-2017
    // §25.7): commits its own members, waits on its own clock.
    task ping(input logic [7:0] n);
        begin
            data <= n;
            req <= 1'b1;
            @(posedge clk);
            while(!ack) @(posedge clk);
            req <= 1'b0;
            @(posedge clk);
        end
    endtask
endinterface

// A machine calling a task through its interface port: HierCallResolution
// splices `ping` as a module task whose members are §6.3 identities, and the
// inlining stems BUS_PING_0/BUS_PING_1 name the per-site states (ADR-0015
// §5.1). The process clocks on the interface's own clock — the identity the
// spliced waits carry.
module fsm_iface6(input logic rst_n, fsm_iface6_bus bus, output logic [1:0] phase);
    (* veriparse_fsm *)
    initial begin
        bus.req <= 1'b0;
        phase <= 2'd0;
        @(posedge bus.clk);
        forever begin
            bus.ping(8'h41);
            phase <= 2'd1;
            bus.ping(8'h42);
            phase <= 2'd2;
            @(posedge bus.clk);
        end
    end
endmodule
