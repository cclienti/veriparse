interface hier_call_err_multi0_bus;
  logic req;

  task ping;
    begin
      req <= 1'b1;
    end
  endtask
endinterface

// A multi-level path: only one level is supported.
module hier_call_err_multi0(hier_call_err_multi0_bus bus);
  initial begin
    top.bus.ping();
  end
endmodule
