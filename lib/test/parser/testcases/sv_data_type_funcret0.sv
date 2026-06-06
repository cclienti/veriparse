module sv_data_type_funcret0;

  function int                f1(input [3:0] a); f1 = a; endfunction
  function bit [7:0]          f2(input [3:0] a); f2 = a; endfunction
  function logic [3:0]        f3(input [3:0] a); f3 = a; endfunction
  function byte               f4(input [3:0] a); f4 = a; endfunction
  function bit signed [15:0]  f5(input [3:0] a); f5 = a; endfunction

endmodule
