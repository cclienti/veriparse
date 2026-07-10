interface e7a_if;
  logic a;
endinterface

interface e7b_if;
  logic a;
endinterface

module e7_user(e7a_if p, output logic o);
  assign o = p.a;
endmodule

module iface_err_type_mismatch0(output logic o);

  e7b_if bus();

  e7_user u(.p(bus), .o(o));

endmodule
