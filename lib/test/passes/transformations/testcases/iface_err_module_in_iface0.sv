module leaf_m(input logic i, output logic o);
  assign o = i;
endmodule

interface bad_if;
  logic x;
  logic y;
  leaf_m u(.i(x), .o(y));
endinterface

module iface_err_module_in_iface0(input logic i, output logic o);

  bad_if bus();

  assign bus.x = i;
  assign o = bus.y;

endmodule
