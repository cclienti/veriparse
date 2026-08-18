interface iface_call_hdrport0_if(input logic clk);
  logic q;

  // The body reads the interface's own header port: a member like any
  // other, spliced as p.clk and retargeted through the connection.
  task sample();
    begin
      q <= clk;
    end
  endtask
endinterface

module iface_call_hdrport0_child(iface_call_hdrport0_if p);

  initial begin
    p.sample();
  end

endmodule

module iface_call_hdrport0(input logic clk);

  iface_call_hdrport0_if bus(.clk(clk));

  iface_call_hdrport0_child u(.p(bus));

endmodule
