interface hs2_if;
  logic req;
  logic ack;
  modport master(output req, input ack);
  modport slave(input req, output ack);
endinterface

module hs2_master(hs2_if.master p, input logic go);
  assign p.req = go;
endmodule

module hs2_slave(hs2_if.slave p, output logic seen);
  assign p.ack = p.req;
  assign seen = p.ack;
endmodule

module iface_modport0(input logic go, output logic seen);

  hs2_if bus();

  hs2_master u_m(.p(bus), .go(go));
  hs2_slave u_s(.p(bus), .seen(seen));

endmodule
