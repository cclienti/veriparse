interface dir_if;
  logic req;
  logic ack;
  modport master(output req, input ack);
endinterface

// Same violation as the continuous-assignment case, reached through a
// procedural lvalue instead (IEEE 1800-2017 25.5).
module dir_master(dir_if.master p, input logic clk, input logic go);
  always @(posedge clk) begin
    p.req <= go;
    p.ack <= go;
  end
endmodule

module iface_err_modport_input_proc0(input logic clk, input logic go);

  dir_if bus();

  dir_master u_m(.p(bus), .clk(clk), .go(go));

endmodule
