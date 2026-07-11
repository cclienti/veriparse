interface bg_if;
  logic d;
endinterface

// The child never references p, so only the connection-time existence check
// can catch the bogus actual.
module bg_user(bg_if p[1:0], output logic o);
  assign o = 1'b0;
endmodule

module iface_err_array_bogus0(output logic o);

  bg_if v[1:0]();

  bg_user u(.p(nosuch), .o(o));

endmodule
