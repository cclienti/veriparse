interface nr2_if #(parameter int W = 8) (input logic clk);

  logic [W-1:0] data;
  logic valid;

  modport consumer (input data, valid);

endinterface

module name_resolution2 (nr2_if bus, input logic clk, nr2_if inherited);

  typedef logic [3:0] nib_t;

  nib_t local_var;

  nr2_if #(.W(16)) u_bus (.clk(clk));
  nr2_if u_arr [1:0] (.clk(clk));

  assign local_var = 4'h0;

endmodule

module name_resolution2b (nr2b_t x);

  typedef logic [3:0] nr2b_t;

endmodule
