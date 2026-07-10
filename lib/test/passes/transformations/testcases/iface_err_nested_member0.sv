interface c5_if;
  logic d;
endinterface

interface l5_if;
  logic up;
  c5_if tx();
endinterface

module l5_user(l5_if p, output logic o);
  assign o = p.tx.nosuch;
endmodule

module iface_err_nested_member0(output logic o);

  l5_if bus();

  l5_user u(.p(bus), .o(o));

endmodule
