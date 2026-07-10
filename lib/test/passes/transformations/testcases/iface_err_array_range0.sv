interface e9_if;
  logic d;
endinterface

module e9_user(e9_if p[3:0], output logic o);
  assign o = p[3].d;
endmodule

module iface_err_array_range0(output logic o);

  e9_if v[1:0]();

  e9_user u(.p(v), .o(o));

endmodule
