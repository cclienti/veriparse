interface handshake_if;

  logic req;
  logic ack;

  modport master (output req, input ack);
  modport slave (input req, output ack);

endinterface

module sv_interface_port0 (handshake_if.slave hs, input logic clk);

  always_ff @(posedge clk)
    hs.ack <= hs.req;

endmodule
