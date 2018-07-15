module instance3 (a, b, c);
   input [3:0]  a, b;
   output [3:0] c;
   my_module inst [3:0] (a, b, c);
endmodule
