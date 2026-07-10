interface e2_if;
  logic a;
  modport mp(input a);
endinterface

module e2_user(e2_if p, output logic o);
  assign o = p.a;
endmodule

module iface_err_conn_modport0(output logic o);

  e2_if bus();

  e2_user u(.p(bus.nomp), .o(o));

endmodule
