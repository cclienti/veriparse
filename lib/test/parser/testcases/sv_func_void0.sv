module sv_func_void0;
  // SystemVerilog void function (no return value) -> VoidType return.
  function void print_it(input int a);
    $display("a=%0d", a);
  endfunction
endmodule
