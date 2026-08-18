interface iface_call_err_modport_vis0_if;
  logic go;
  logic ack;
  logic hidden;

  // Touches a member the modport does not list: after the splice the
  // reference is a caller-side access and the visibility check refuses it.
  task pulse();
    begin
      hidden <= 1'b1;
    end
  endtask

  modport dev(input go, output ack);
endinterface

module iface_call_err_modport_vis0_child(iface_call_err_modport_vis0_if.dev p);
  initial begin
    p.pulse();
  end
endmodule

module iface_call_err_modport_vis0;

  iface_call_err_modport_vis0_if bus();

  iface_call_err_modport_vis0_child u(.p(bus.dev));

endmodule
