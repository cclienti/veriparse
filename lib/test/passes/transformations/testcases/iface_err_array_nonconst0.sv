interface e10_if;
  logic d;
endinterface

module e10_user(e10_if p[1:0], input logic sel, output logic o);
  assign o = p[sel].d;
endmodule

module iface_err_array_nonconst0(input logic sel, output logic o);

  e10_if v[1:0]();

  e10_user u(.p(v), .sel(sel), .o(o));

endmodule
