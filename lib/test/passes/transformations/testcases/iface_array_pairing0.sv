interface pr_if;
  logic d;
endinterface

module pr_user(pr_if p, output logic o);
  assign o = p.d;
endmodule

// Opposite-direction ranges: elements pair left-to-left (IEEE 1800-2017
// 23.3.3.5) — child u[0:1] left element u0 pairs with actual v[1:0] left
// element v1.
module iface_array_pairing0(input logic a, input logic b, output logic [1:0] o);

  pr_if v[1:0]();

  assign v[1].d = a;
  assign v[0].d = b;

  pr_user u[0:1](.p(v), .o(o));

endmodule
