interface handshake_if;
  logic req;
  logic ack;
  modport master(output req, input ack);
endinterface

module synthesizable_check_virtual0 (input logic clk);

  // Virtual interface: a dynamic handle, not synthesizable (IEEE 1800-2017 §25.9).
  virtual handshake_if v;

endmodule
