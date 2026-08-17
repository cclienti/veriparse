interface sv_hier_taskcall0_bus;
  logic req;
  logic ack;

  task ping(input logic [7:0] n);
    begin
      req <= 1'b1;
    end
  endtask

  task reset();
    begin
      req <= 1'b0;
    end
  endtask

  task tick;
    begin
      req <= ~req;
    end
  endtask
endinterface

module sv_hier_taskcall0(sv_hier_taskcall0_bus bus);

  initial begin
    bus.ping(8'd1);
    bus.reset();
    bus.tick;
  end

endmodule
