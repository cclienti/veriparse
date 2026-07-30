// Declaration lifetime (IEEE 1800-2017 §13.3.1, A.1.2:
// `module_keyword [ lifetime ] module_identifier`): a module, like an
// interface or a package, may be declared `automatic` or `static`.
// Subroutines defined inside inherit that default when they state none;
// resolving the effective lifetime is a later pass — the parser only records
// what was written.
module automatic sv_lifetime0_auto (input logic c, output logic o);
   function int f(input int x);
      int tmp;
      begin
         tmp = x + 1;
         f = tmp;
      end
   endfunction

   task t(input int x, output int y);
      begin
         y = x;
      end
   endtask

   assign o = c;
endmodule

module static sv_lifetime0_static (input logic c, output logic o);
   function automatic int g(input int x);
      g = x;
   endfunction
   assign o = c;
endmodule

// No lifetime keyword: stays NONE (static by default, §13.3.1).
module sv_lifetime0 (input logic c, output logic o);
   assign o = c;
endmodule
