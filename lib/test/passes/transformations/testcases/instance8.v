module instance8;
   my_mod      mod0();
   my_mod #(2) mod1();
   my_mod #(3, 4) mod2();
   my_mod #(.X(5)) mod3();
   my_mod #(.Y(7), .X(6)) mod4();
   my_mod #(.Y(), .X()) mod5();
endmodule

module my_mod();
   parameter X=3;
   parameter Y=2*X;

   initial begin
      $display("X=%0d", X);
      $display("Y=%0d\n", Y);
   end
endmodule
