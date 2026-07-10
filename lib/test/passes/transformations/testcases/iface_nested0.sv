interface ch_if;
  logic v;
  logic [7:0] d;
endinterface

interface lk_if;
  logic up;
  ch_if tx();
  ch_if rx();
endinterface

module iface_nested0(input logic a, input logic [7:0] din,
                     output logic [7:0] dout, output logic o);

  lk_if bus();

  assign bus.up = a;
  assign bus.tx.v = a;
  assign bus.tx.d = din;
  assign bus.rx.v = bus.tx.v;
  assign bus.rx.d = bus.tx.d;
  assign dout = bus.rx.d;
  assign o = bus.rx.v & bus.up;

endmodule
