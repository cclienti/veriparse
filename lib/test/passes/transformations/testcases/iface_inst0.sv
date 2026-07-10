interface hs_if;
  logic req;
  logic ack;
endinterface

module iface_inst0(input logic in, output logic out);

  hs_if bus();

  assign bus.req = in;
  assign bus.ack = bus.req;
  assign out = bus.ack;

endmodule
