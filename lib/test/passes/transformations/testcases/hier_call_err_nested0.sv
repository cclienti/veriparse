interface hier_call_err_nested0_bus;
  logic req;

  task pulse;
    begin
      req <= 1'b1;
    end
  endtask

  // Calls a sibling subroutine: the splice carries no dependency closure
  // (v1).
  task ping;
    begin
      pulse;
    end
  endtask
endinterface

module hier_call_err_nested0(hier_call_err_nested0_bus bus);
  initial begin
    bus.ping();
  end
endmodule
