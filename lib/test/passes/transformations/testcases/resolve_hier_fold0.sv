interface resolve_hier_fold0_if;
  logic [7:0] data;

  function logic [7:0] gv();
    return 8'h2A;
  endfunction
endinterface

// The spliced function joins the module's function dictionary: a
// constant-argument call folds exactly like a local one would.
module resolve_hier_fold0(resolve_hier_fold0_if bus, output logic [7:0] y);

  assign y = bus.gv();

endmodule
