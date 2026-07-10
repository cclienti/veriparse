interface d_if;
  logic d;
endinterface

module d_user(d_if p, output logic o);
  assign o = p.d;
endmodule

module iface_elem0(input logic i, output logic o);

  d_if v[1:0]();

  assign v[0].d = i;
  assign v[1].d = ~i;

  d_user u(.p(v[1]), .o(o));

endmodule
