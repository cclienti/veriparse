interface sv_modport0;

  logic req;
  logic ack;
  logic [7:0] data;

  modport master (output req, data, input ack);
  modport slave (input req, data, output ack);

endinterface
