interface o_if;
  logic d;
endinterface

module o_driver(input logic i, output logic o);
  assign o = i;
endmodule

module iface_out0(input logic i, output logic q);

  o_if bus();

  o_driver u(.i(i), .o(bus.d));

  assign q = bus.d;

endmodule
