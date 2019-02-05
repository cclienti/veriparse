module my_module (a, b, c, d);
   input a, b, c, d;

   always @(*) begin
      $display(a);
      $display(b);
      $display(c);
      $display(d);
   end
endmodule

module instance9 (a, b, c, d);
   input [3:0] a, b, c, d;
   my_module inst0 [3:0] (a, , , );
   my_module inst1 [3:0] (, b, , );
   my_module inst2 [3:0] (, , c, );
   my_module inst3 [3:0] (, , , d);
endmodule
