module instance3 (a, b, c);
   input [3:0]  a, b;
   output [3:0] c;
   my_module3 inst [3:0] (a, b, c);
endmodule

module my_module3 (ia, ib, oc);
   input wire ia, ib;
   output reg oc;

   always @(*) oc = ia && ib;
endmodule
