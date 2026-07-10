interface p_if;
  logic d;
endinterface

module iface_err_top_port0(p_if bus, output logic o);

  assign o = bus.d;

endmodule
