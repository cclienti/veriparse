interface cyc_if;
  logic d;
  cyc_if sub();
endinterface

module iface_err_nested_cycle0(input logic i, output logic o);

  cyc_if bus();

  assign bus.d = i;
  assign o = bus.d;

endmodule
