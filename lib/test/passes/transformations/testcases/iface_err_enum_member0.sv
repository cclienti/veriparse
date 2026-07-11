interface en_if;
  logic st;
  enum logic [0:0] {EN_IDLE, EN_RUN} mode;
endinterface

module en_user(en_if p, output logic o);
  assign o = p.EN_IDLE;
endmodule

module iface_err_enum_member0(output logic o);

  en_if bus();

  en_user u(.p(bus), .o(o));

endmodule
