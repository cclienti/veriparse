interface c6_if;
  logic d;
endinterface

interface l6_if;
  logic up;
  c6_if tx();
  modport mp(input up);
endinterface

module l6_user(l6_if.mp p, output logic o);
  assign o = p.tx.d;
endmodule

module iface_err_nested_modport0(output logic o);

  l6_if bus();

  l6_user u(.p(bus), .o(o));

endmodule
