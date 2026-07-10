interface e6_if;
  logic a;
endinterface

module e6_user(e6_if p, output logic o);
  assign o = p.a;
endmodule

module iface_err_unconnected0(output logic o);

  e6_if bus();

  e6_user u(.o(o));

endmodule
