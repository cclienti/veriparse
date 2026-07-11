interface sh_if;
  logic d;
endinterface

module sh_user(sh_if p, input logic x, output logic o);
  assign o = p.d ^ x;
endmodule

// A single (scalar) interface instance connects to EACH element of the
// child instance array (IEEE 1800-2017 23.3.3.5).
module iface_array_shared0(input logic a, input logic [1:0] x, output logic [1:0] o);

  sh_if bus();

  assign bus.d = a;

  sh_user u[1:0](.p(bus), .x(x), .o(o));

endmodule
