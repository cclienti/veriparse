interface pa_if;
  logic d;
  modport mp(input d);
endinterface

module sv_interface_port_array0 (
    pa_if p[3:0],
    pa_if.mp q[1:0],
    input logic clk
);

endmodule
