interface hier_call_port0_bus(input logic clk);
  logic       req;
  logic       ack;
  logic [7:0] data;

  // A multi-cycle handshake helper living with the bus it drives: waits on
  // the interface's own clock, drives its own members.
  task ping(input logic [7:0] n);
    begin
      data <= n;
      req <= 1'b1;
      @(posedge clk);
      while(!ack) @(posedge clk);
      req <= 1'b0;
    end
  endtask

  // A formal shadowing the member `data`: only the free `data` above is a
  // member reference; this one must stay the formal.
  task shadow(input logic [7:0] data);
    begin
      req <= data[0];
    end
  endtask

  function logic [7:0] gv();
    return data;
  endfunction
endinterface

module hier_call_port0(input logic rst_n, hier_call_port0_bus bus, output logic [7:0] y);

  assign y = bus.gv() + 8'd1;

  initial begin
    bus.ping(8'h41);
    bus.ping(8'h42);
    bus.shadow(8'h01);
  end

endmodule
