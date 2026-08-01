interface dir_if;
  logic [3:0] req;
  logic [3:0] ack;
  modport master(output req, input ack);
endinterface

// A bit-select does not launder the direction: the target of the assignment is
// still 'ack', which the master modport declares input (IEEE 1800-2017 25.5).
// This is the shape the Indirect carve-out must keep in write context — it
// clears the flag on the index, never on the selected variable.
module dir_master(dir_if.master p, input logic clk, input logic go);
  always @(posedge clk) begin
    p.req[0] <= go;
    p.ack[0] <= go;
  end
endmodule

module iface_err_modport_input_select0(input logic clk, input logic go);

  dir_if bus();

  dir_master u_m(.p(bus), .clk(clk), .go(go));

endmodule
