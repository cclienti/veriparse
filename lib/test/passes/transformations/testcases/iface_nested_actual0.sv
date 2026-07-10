interface c4_if;
  logic d;
endinterface

interface l4_if;
  logic up;
  c4_if tx();
endinterface

module c4_user(c4_if c, output logic o);
  assign o = c.d;
endmodule

module l4_mid(l4_if p, output logic o);
  c4_user u(.c(p.tx), .o(o));
endmodule

module iface_nested_actual0(input logic a, output logic o0, output logic o1);

  l4_if bus();

  assign bus.up = a;
  assign bus.tx.d = a;

  c4_user u0(.c(bus.tx), .o(o0));
  l4_mid um(.p(bus), .o(o1));

endmodule
