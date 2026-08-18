interface hier_call_generate0_if;
  logic tick;

  // A bare generate region is semantically transparent (IEEE 1800-2017
  // §27.3): the task it holds is a subroutine of the interface.
  generate
    task bump();
      begin
        tick <= 1'b1;
      end
    endtask
  endgenerate
endinterface

module hier_call_generate0(hier_call_generate0_if bus);

  initial begin
    bus.bump();
  end

endmodule
