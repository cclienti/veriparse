interface link_if;
  logic d;
  logic q;
endinterface

module link_producer(link_if p, input logic v);
  assign p.d = v;
endmodule

module link_consumer(link_if p, output logic o);
  assign o = p.d;
endmodule

module iface_port0(input logic v, output wire o);

  link_if bus();

  link_producer u_p(.p(bus), .v(v));
  link_consumer u_c(.p(bus), .o(o));

endmodule
