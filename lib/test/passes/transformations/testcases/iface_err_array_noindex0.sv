interface e11_if;
  logic d;
endinterface

module e11_user(e11_if p[1:0], output logic o);
  assign o = p.d;
endmodule

module iface_err_array_noindex0(output logic o);

  e11_if v[1:0]();

  e11_user u(.p(v), .o(o));

endmodule
