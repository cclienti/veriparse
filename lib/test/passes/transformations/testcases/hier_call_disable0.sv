interface hier_call_disable0_if;
  logic req;
  logic done;

  // `disable` targets live in the block namespace (IEEE 1800-2017 §9.6.2):
  // a label sharing a member's name is not that member. Neither disable
  // target is rewritten; the value references still are.
  task ping();
    begin
      begin : req
        done <= 1'b1;
        disable req;
      end
      begin : quit
        req <= 1'b0;
        disable quit;
      end
    end
  endtask
endinterface

module hier_call_disable0(hier_call_disable0_if bus);

  initial begin
    bus.ping();
  end

endmodule
