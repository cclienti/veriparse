module instance10;
   parameter integer MY_X = 5;
   parameter integer MY_Y = 6;
   parameter integer MY_Z = 7;

   my_mod mod0();
   my_mod #(MY_X) mod1();
   my_mod #(MY_X, ,) mod2();
   my_mod #(, MY_Y,) mod3();
   my_mod #(, , MY_Z) mod4();
   my_mod #(MY_X, MY_Y, MY_Z) mod5();
   my_mod #(.X(MY_X)) mod6();
   my_mod #(.Y(MY_Y), .X(MY_X)) mod7();
   my_mod #(.Y(8), .X(MY_X)) mod8();
   my_mod #(.Y(8), .X(MY_X), .Z(MY_Z)) mod9();
endmodule

module my_mod();
   parameter X=3;
   parameter Y=2*X;
   parameter Z=3*X;

   initial begin
      $display("X=%0d", X);
      $display("Y=%0d", Y);
      $display("Z=%0d\n", Z);
   end
endmodule
