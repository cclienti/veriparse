module my_module (a, c, b);
   input  a, b;
   output c;
   assign c = a & b;
endmodule

module top (a, b, c);
   input [3:0]  a, b;
   output [3:0] c;
   my_module inst [3:0] (a, ,b);
endmodule
