interface hier_call_err_typeparam0_if;
  parameter type T = logic;
  logic go;

  // The body references an interface type parameter: outside the v1
  // closure, refused naming its kind.
  task ping();
    begin
      go <= $bits(T);
    end
  endtask
endinterface

module hier_call_err_typeparam0(hier_call_err_typeparam0_if bus);
  initial begin
    bus.ping();
  end
endmodule
