interface hier_call_err_param0_bus;
  localparam WIDTH = 8;
  logic [WIDTH-1:0] data;

  // The body references an interface localparam: the splice cannot carry it
  // (v1 closure).
  task ping;
    begin
      data <= WIDTH[7:0];
    end
  endtask
endinterface

module hier_call_err_param0(hier_call_err_param0_bus bus);
  initial begin
    bus.ping();
  end
endmodule
