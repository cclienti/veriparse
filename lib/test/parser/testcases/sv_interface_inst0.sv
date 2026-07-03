interface bus_if #(parameter int W = 8) (input logic clk);

  logic [W-1:0] data;

endinterface

module sv_interface_inst0 (input logic clk);

  bus_if #(.W(16)) u_bus (.clk(clk));
  bus_if u_arr [3:0] (.clk(clk));

endmodule
