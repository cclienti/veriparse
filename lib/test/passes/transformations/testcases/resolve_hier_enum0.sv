interface resolve_hier_enum0_if;
  logic en;
  logic [7:0] acc;

  // An inline enum and a bounded loop inside the body: the splice lands
  // after the module's own enum elaboration and loop unrolling ran, so
  // those passes must run again over the spliced clone.
  task kick();
    begin
      enum logic [0:0] {IDLE, GO} st;
      st = GO;
      en <= st;
    end
  endtask

  task add2(input logic [7:0] n);
    begin
      integer i;
      for(i = 0; i < 2; i = i + 1) begin
        acc <= acc + n;
      end
    end
  endtask
endinterface

module resolve_hier_enum0(resolve_hier_enum0_if bus, output logic y, output logic z);

  assign y = bus.en;
  assign z = bus.acc[0];

  initial begin
    bus.kick();
    bus.add2(8'd2);
  end

endmodule
