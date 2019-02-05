module my_module (a, b, c, d);
   output a, b, c, d;

   always @(*) begin
      a = 0;
      b = 0;
      c = 0;
      d = 0;
   end


endmodule

module top0 (a, b, c, d);
   output [3:0] a, b, c, d;
   my_module inst [3:0] (a, , ,);
endmodule

module top1 (a, b, c, d);
   output [3:0] a, b, c, d;
   my_module inst [3:0] (, b, ,);
endmodule

module top2 (a, b, c, d);
   output [3:0] a, b, c, d;
   my_module inst [3:0] (, , c,);
endmodule

module top3 (a, b, c, d);
   output [3:0] a, b, c, d;
   my_module inst [3:0] (, , , d);
endmodule
