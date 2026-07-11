interface cap_if;
  logic x;
endinterface

// The second port is deliberately named like the first port's actual: the
// one-walk rewrite must not re-capture the first rewrite's result.
module cap_child(cap_if p, cap_if bus, output logic o0, output logic o1);
  assign o0 = p.x;
  assign o1 = bus.x;
endmodule

module iface_alias_capture0(input logic a, input logic b,
                            output logic o0, output logic o1);

  cap_if bus();
  cap_if other();

  assign bus.x = a;
  assign other.x = b;

  cap_child u(.p(bus), .bus(other), .o0(o0), .o1(o1));

endmodule
