interface m_if;
  logic a;
  modport mp(input a, output nothere);
endinterface

module iface_err_bad_modport0(input logic i, output logic o);

  m_if bus();

  assign bus.a = i;
  assign o = bus.a;

endmodule
