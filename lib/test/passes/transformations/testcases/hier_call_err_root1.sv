interface hier_call_err_root1_bus;
  logic req;

  task ping;
    begin
      req <= 1'b1;
    end
  endtask
endinterface

// The root names a locally instantiated interface: the splice would collide
// with the flattener's instance-prefix rename of the same subroutine.
module hier_call_err_root1;
  hier_call_err_root1_bus bus();

  initial begin
    bus.ping();
  end
endmodule
