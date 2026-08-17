interface hier_call_err_exprtask0_bus;
  logic req;

  task ping;
    begin
      req <= 1'b1;
    end
  endtask
endinterface

// A task in expression position: a task returns no value (IEEE 1800-2017
// 13.5).
module hier_call_err_exprtask0(hier_call_err_exprtask0_bus bus, output logic y);
  assign y = bus.ping();
endmodule
