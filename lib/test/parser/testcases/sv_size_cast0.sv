module sv_size_cast0;
  // SystemVerilog size cast `N'(expr)` -> SizeCast.
  logic [7:0]  a;
  logic [15:0] b;
  assign b = 16'(a);
endmodule
