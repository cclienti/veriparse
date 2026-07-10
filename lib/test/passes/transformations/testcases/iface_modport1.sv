interface hs3_if;
  logic req;
  logic ack;
  modport master(output req, input ack);
endinterface

module hs3_user(hs3_if p, input logic go, output logic done);
  assign p.req = go;
  assign done = p.ack;
endmodule

module iface_modport1(input logic go, output logic done);

  hs3_if bus();

  assign bus.ack = bus.req;

  hs3_user u(.p(bus.master), .go(go), .done(done));

endmodule
