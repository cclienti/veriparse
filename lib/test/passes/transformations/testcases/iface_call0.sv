interface iface_call0_if;
  logic       en;
  logic [7:0] data;
  logic [7:0] acc;

  task bump(input logic [7:0] n);
    begin
      acc <= acc + n;
      en <= 1'b1;
    end
  endtask

  function logic [7:0] gv();
    return data;
  endfunction
endinterface

// A child calling its interface port's subroutines: HierCallResolution
// splices them during the child's resolution, before the port dissolves —
// the flattener then sees only bare calls plus p.member references it
// already retargets, and prefixes the spliced declarations per instance.
module iface_call0_child(iface_call0_if p, input logic [7:0] k, output logic [7:0] snap);

  assign snap = p.gv() + k;

  initial begin
    p.bump(8'h11);
    p.bump(8'h22);
  end

endmodule

module iface_call0(input logic [7:0] k, output logic [7:0] s0, output logic [7:0] s1);

  iface_call0_if bus0();
  iface_call0_if bus1();

  iface_call0_child u0(.p(bus0), .k(k), .snap(s0));
  iface_call0_child u1(.p(bus1), .k(k), .snap(s1));

endmodule
