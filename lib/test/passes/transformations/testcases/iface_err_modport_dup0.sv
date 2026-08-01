interface dup_if;
  logic d;
  // Listing a member twice leaves no defined direction for it: whichever entry
  // wins decides whether driving 'd' is legal, so the declaration is rejected
  // rather than resolved by source order (IEEE 1800-2017 25.5).
  modport m(output d, input d);
endinterface

module dup_master(dup_if.m p, input logic go);
  assign p.d = go;
endmodule

module iface_err_modport_dup0(input logic go);

  dup_if bus();

  dup_master u_m(.p(bus), .go(go));

endmodule
