module logic_out0_leaf(input logic i, output logic o);
  assign o = i;
endmodule

module logic_out0(input logic i, output logic o);

  logic mid;

  logic_out0_leaf u0(.i(i), .o(mid));

  assign o = mid;

endmodule
