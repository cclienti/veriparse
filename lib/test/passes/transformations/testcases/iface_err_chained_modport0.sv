interface cm_if;
  logic d;
  modport mp(input d);
endinterface

module cm_leaf(cm_if p, output logic o);
  assign o = p.d;
endmodule

module cm_mid(cm_if.badmp p2, output logic o);
  cm_leaf u(.p(p2), .o(o));
endmodule

module iface_err_chained_modport0(output logic o);

  cm_if bus();

  cm_mid um(.p2(bus), .o(o));

endmodule
