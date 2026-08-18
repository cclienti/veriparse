interface iface_call_err_modport_dir0_if;
  logic go;
  logic ack;

  // Writes a member the modport declares input: after the splice the write
  // is a caller-side drive and the direction check refuses it.
  task pulse();
    begin
      go <= 1'b1;
    end
  endtask

  modport dev(input go, output ack);
endinterface

module iface_call_err_modport_dir0_child(iface_call_err_modport_dir0_if.dev p);
  initial begin
    p.pulse();
  end
endmodule

module iface_call_err_modport_dir0;

  iface_call_err_modport_dir0_if bus();

  iface_call_err_modport_dir0_child u(.p(bus.dev));

endmodule
