// Effective subroutine lifetime (IEEE 1800-2017 §13.3.1/§13.4.2): a task or
// function defined in a module/interface/package defaults to STATIC, unless
// it declares `automatic` itself or the enclosing declaration is `automatic`.
// Resolution makes the inherited value explicit on every subroutine.
module automatic default_resolution_lifetime0_auto (input logic c, output logic o);
   // inherits AUTOMATIC from the module
   function int inherits_auto(input int x);
      inherits_auto = x;
   endfunction
   // states its own, overriding the enclosing default
   function static int states_static(input int x);
      states_static = x;
   endfunction
   task inherits_auto_task(input int x, output int y);
      y = x;
   endtask
   assign o = c;
endmodule

module static default_resolution_lifetime0_static (input logic c, output logic o);
   function int inherits_static(input int x);
      inherits_static = x;
   endfunction
   function automatic int states_auto(input int x);
      states_auto = x;
   endfunction
   assign o = c;
endmodule

// No lifetime on the module: subroutines are static by default (§13.3.1).
module default_resolution_lifetime0 (input logic c, output logic o);
   function int defaults_static(input int x);
      defaults_static = x;
   endfunction
   assign o = c;
endmodule
