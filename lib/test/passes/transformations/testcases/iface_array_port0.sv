interface c2_if;
  logic d;
endinterface

module c2_sum(c2_if p[1:0], output logic o);
  assign o = p[0].d ^ p[1].d;
endmodule

module iface_array_port0(input logic i0, input logic i1, output logic o);

  c2_if v[1:0]();

  assign v[0].d = i0;
  assign v[1].d = i1;

  c2_sum u(.p(v), .o(o));

endmodule
