module my_module (a, b, c, d);
   parameter W=0, X=1, Y=2, Z=2;
   output wire [3:0] a, b, c, d;

   always @(*) begin
      a = W;
      b = X;
      c = Y;
      d = Z;
   end

endmodule

module top0 (a, b, c, d);
   output [3:0] a, b, c, d;
   my_module #(4, , , ) inst [3:0] (a, , ,);
endmodule

module top1 (a, b, c, d);
   output [3:0] a, b, c, d;
   my_module #( , 3, ,) inst [3:0] (, b, ,);
endmodule

module top2 (a, b, c, d);
   output [3:0] a, b, c, d;
   my_module #( , , 2,) inst [3:0] (, , c,);
endmodule

module top3 (a, b, c, d);
   output [3:0] a, b, c, d;
   my_module #( , , , 1) inst [3:0] (, , , d);
endmodule
