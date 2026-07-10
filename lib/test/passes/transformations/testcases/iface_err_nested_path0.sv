interface e8_if;
  logic a;
endinterface

module e8_user(e8_if p, output logic o);
  assign o = p.a.b;
endmodule

module iface_err_nested_path0(output logic o);

  e8_if bus();

  e8_user u(.p(bus), .o(o));

endmodule
