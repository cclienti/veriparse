module instance11;
   parameter integer PARAM_0 = 6;
   parameter integer PARAM_1 = 5;

   my_mod mod0(PARAM_0);
   my_mod mod1(PARAM_1[7:0]);
endmodule

module my_mod(input wire [7:0] x);

   always @(*) begin
      $display("x=%0d", x);
   end
endmodule
