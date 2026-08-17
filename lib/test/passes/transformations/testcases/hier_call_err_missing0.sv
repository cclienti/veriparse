interface hier_call_err_missing0_bus;
  logic req;
endinterface

// The interface declares no such subroutine.
module hier_call_err_missing0(hier_call_err_missing0_bus bus);
  initial begin
    bus.nosuch();
  end
endmodule
