interface e5_if;
  logic a;
endinterface

module e5_user(e5_if p, output logic o);
  assign o = p.nosuch;
endmodule

module iface_err_no_member0(output logic o);

  e5_if bus();

  e5_user u(.p(bus), .o(o));

endmodule
