interface sv_interface1
  #(parameter int WIDTH = 8)
  (input logic clk, input logic rst_n);

  logic [WIDTH-1:0] data;
  logic valid;

  assign data = {WIDTH{valid}};

endinterface : sv_interface1
