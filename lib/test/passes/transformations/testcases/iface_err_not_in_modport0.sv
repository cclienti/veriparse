interface e4_if;
  logic a;
  logic b;
  modport mp(input a);
endinterface

module e4_user(e4_if.mp p, output logic o);
  assign o = p.b;
endmodule

module iface_err_not_in_modport0(output logic o);

  e4_if bus();

  e4_user u(.p(bus), .o(o));

endmodule
