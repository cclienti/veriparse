interface e1_if;
  logic a;
  modport mp(input a);
endinterface

module e1_user(e1_if.nomp p, output logic o);
  assign o = p.a;
endmodule

module iface_err_hdr_modport0(output logic o);

  e1_if bus();

  e1_user u(.p(bus), .o(o));

endmodule
