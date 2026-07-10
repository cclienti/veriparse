interface b_if;
  logic d;
endinterface

module b_drv(b_if p, input logic x);
  assign p.d = x;
endmodule

module iface_array1(input logic [3:0] x, output logic [3:0] y);

  b_if v[3:0]();

  b_drv u[3:0](.p(v), .x(x));

  assign y = {v[3].d, v[2].d, v[1].d, v[0].d};

endmodule
