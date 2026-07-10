interface c3_if;
  logic d;
endinterface

interface l3_if;
  logic up;
  c3_if tx();
endinterface

module l3_user(l3_if p, output logic o);
  assign p.tx.d = p.up;
  assign o = p.tx.d;
endmodule

module iface_nested_port0(input logic a, output logic o);

  l3_if bus();

  assign bus.up = a;

  l3_user u(.p(bus), .o(o));

endmodule
