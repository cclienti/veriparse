`ifdef MODE_B
module variant_b(input wire i, output wire o);
  assign o = ~i;
endmodule
`else
module variant_a(input wire i, output wire o);
  assign o = i;
endmodule
`endif
