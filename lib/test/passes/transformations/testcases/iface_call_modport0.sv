interface iface_call_modport0_if;
  logic go;
  logic ack;

  // Writes only the member the modport declares output: the spliced body
  // meets the caller-side direction check.
  task reply();
    begin
      ack <= 1'b1;
    end
  endtask

  modport dev(input go, output ack);
endinterface

module iface_call_modport0_child(iface_call_modport0_if.dev p);
  initial begin
    p.reply();
  end
endmodule

module iface_call_modport0;

  iface_call_modport0_if bus();

  iface_call_modport0_child u(.p(bus.dev));

endmodule
