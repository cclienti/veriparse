interface dir_if;
  logic req;
  logic ack;
  modport master(output req, input ack);
endinterface

// 'ack' is an input of the master modport: the module may read it, never drive
// it (IEEE 1800-2017 25.5 — the directions are declared as if inside the
// module, so an input behaves like a module input).
module dir_master(dir_if.master p, input logic go);
  assign p.req = go;
  assign p.ack = go;
endmodule

module iface_err_modport_input_assign0(input logic go);

  dir_if bus();

  dir_master u_m(.p(bus), .go(go));

endmodule
