interface hier_call_err_indexed0_bus;
  logic req;

  task ping;
    begin
      req <= 1'b1;
    end
  endtask
endinterface

// An indexed root has no static identity.
module hier_call_err_indexed0(hier_call_err_indexed0_bus bus);
  initial begin
    bus[0].ping();
  end
endmodule
