interface handshake_if;
  logic req;
  logic ack;
  modport master(output req, input ack);
endinterface

// A non-virtual, modport-qualified interface port: synthesizable RTL.
module synthesizable_check_ok0 (handshake_if.master bus, input logic clk);

endmodule
