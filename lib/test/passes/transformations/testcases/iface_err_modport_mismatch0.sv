interface e3_if;
  logic a;
  modport m1(input a);
  modport m2(output a);
endinterface

module e3_user(e3_if.m1 p, output logic o);
  assign o = p.a;
endmodule

module iface_err_modport_mismatch0(output logic o);

  e3_if bus();

  e3_user u(.p(bus.m2), .o(o));

endmodule
