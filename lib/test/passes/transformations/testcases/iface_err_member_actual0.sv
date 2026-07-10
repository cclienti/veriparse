interface e12_if;
  logic d;
endinterface

module e12_user(e12_if p, output logic o);
  assign o = p.d;
endmodule

module iface_err_member_actual0(output logic o);

  e12_if v[1:0]();

  e12_user u(.p(v[0].d), .o(o));

endmodule
