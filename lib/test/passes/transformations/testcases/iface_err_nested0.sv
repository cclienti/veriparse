interface inner_if;
  logic z;
endinterface

interface outer_if;
  logic w;
  inner_if sub();
endinterface

module iface_err_nested0(input logic i, output logic o);

  outer_if bus();

  assign bus.w = i;
  assign o = bus.w;

endmodule
