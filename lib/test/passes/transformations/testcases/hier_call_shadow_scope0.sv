interface hier_call_shadow_scope0_if;
  logic req;
  logic ack;

  // A block-local shadows the member `req` inside its own block only: the
  // free references before and after keep their member identity, and the
  // member `ack` rewrites everywhere.
  task ping();
    begin
      req <= 1'b1;
      begin : b
        logic req;
        req = 1'b0;
        ack <= req;
      end
      ack <= req;
    end
  endtask
endinterface

module hier_call_shadow_scope0(hier_call_shadow_scope0_if bus);

  initial begin
    bus.ping();
  end

endmodule
