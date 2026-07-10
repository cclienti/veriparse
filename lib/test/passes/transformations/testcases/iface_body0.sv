interface vr_if;
  logic valid;
  logic ready;
  logic fire;
  assign fire = valid & ready;
endinterface

module iface_body0(input logic v0, input logic r0, input logic v1, input logic r1,
                   output logic f0, output logic f1);

  vr_if a();
  vr_if b();

  assign a.valid = v0;
  assign a.ready = r0;
  assign b.valid = v1;
  assign b.ready = r1;
  assign f0 = a.fire;
  assign f1 = b.fire;

endmodule
