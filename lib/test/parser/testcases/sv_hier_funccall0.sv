interface sv_hier_funccall0_bus;
  logic [7:0] data;

  function logic [7:0] gv();
    return data;
  endfunction

  function logic [7:0] add(input logic [7:0] a, input logic [7:0] b);
    return a + b;
  endfunction
endinterface

module sv_hier_funccall0(sv_hier_funccall0_bus bus, output logic [7:0] y, output logic [7:0] z);

  assign y = bus.gv() + 8'd1;
  assign z = bus.add(bus.data, 8'd2);

endmodule
