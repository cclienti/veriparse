interface c_if;
  logic d;
endinterface

module chain_leaf(c_if p, output logic o);
  assign o = p.d;
endmodule

module chain_mid(c_if p2, output logic o);
  chain_leaf u(.p(p2), .o(o));
endmodule

module iface_chain0(input logic v, output logic o);

  c_if bus();

  assign bus.d = v;

  chain_mid u_mid(.p2(bus), .o(o));

endmodule
