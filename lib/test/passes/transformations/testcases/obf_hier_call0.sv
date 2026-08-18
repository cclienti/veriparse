interface obf_hier_call0_if;
  logic go;

  task ping();
    begin
      go <= 1'b1;
    end
  endtask
endinterface

// A hierarchical call names a declaration of another scope: renaming its
// leaf against this module's declarations would corrupt the reference, so
// the module is refused.
module obf_hier_call0(obf_hier_call0_if bus);

  task pong;
    begin
    end
  endtask

  initial begin
    pong;
    bus.ping();
  end

endmodule
