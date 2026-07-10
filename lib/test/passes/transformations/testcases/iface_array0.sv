interface a_if;
  logic d;
endinterface

module iface_array0(input logic [3:0] in, output logic [3:0] out);

  a_if v[3:0]();

  assign v[0].d = in[0];
  assign v[1].d = in[1];
  assign v[2].d = in[2];
  assign v[3].d = in[3];

  assign out = {v[3].d, v[2].d, v[1].d, v[0].d};

endmodule
