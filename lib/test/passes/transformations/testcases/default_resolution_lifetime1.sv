// Effective lifetime inherited from an INTERFACE (IEEE 1800-2017 §13.3.1
// names module, interface, program and package alike): a subroutine defined
// in an `automatic` interface is automatic unless it says otherwise, and one
// in a plain interface is static.
interface automatic default_resolution_lifetime1_auto_if (input logic clk);
   logic v;
   function int inherits_auto(input int x);
      inherits_auto = x;
   endfunction
   task inherits_auto_task(input int x, output int y);
      y = x;
   endtask
   function static int states_static(input int x);
      states_static = x;
   endfunction
endinterface

interface default_resolution_lifetime1_if (input logic clk);
   logic v;
   function int inherits_static(input int x);
      inherits_static = x;
   endfunction
   function automatic int states_auto(input int x);
      states_auto = x;
   endfunction
endinterface

module default_resolution_lifetime1 (input logic c, output logic o);
   assign o = c;
endmodule
