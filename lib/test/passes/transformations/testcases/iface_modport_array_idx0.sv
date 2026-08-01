interface a_if;
  logic rdy;
  modport dst(output rdy);
endinterface

interface s_if;
  logic sel;
  modport rd(input sel);
endinterface

// An arrayed interface port carries its index on the reference's hier label,
// not in an Indirect node. That index is still a read: 'sel' is an input of
// modport 'rd' and selecting with it drives nothing (IEEE 1800-2017 25.5).
// The port array is chained through mid, which is what keeps a non-constant
// index alive down to inner.
module inner(a_if.dst p[1:0], s_if.rd q);
  assign p[q.sel].rdy = 1'b1;
endmodule

module mid(a_if.dst p[1:0], s_if.rd q);
  inner u(.p(p), .q(q));
endmodule

module iface_modport_array_idx0(input logic s, output logic o);

  a_if bus[1:0]();
  s_if sc();

  assign sc.sel = s;

  mid u(.p(bus), .q(sc));

  assign o = bus[0].rdy;

endmodule
