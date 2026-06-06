module sv_data_type_funcport0;

  function int f(input int a, input bit [3:0] b, input logic c, input integer i);
    f = a;
  endfunction

  task t(input byte x, input shortint y);
  endtask

endmodule
