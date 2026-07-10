interface clk_if(input logic clk);
  logic tick;
  always @(posedge clk) tick <= ~tick;
endinterface

module iface_header0(input logic clk, output logic t);

  clk_if bus(.clk(clk));

  assign t = bus.tick;

endmodule
