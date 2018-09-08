module instance7(a, b, c);
   input wire [7:0] a;
   input wire [7:0] b;
   input wire [7:0] c;

   my_mod0 my_mod0_inst
   (
      .x (a),
      .y (b)
   );

   my_mod1 my_mod1_inst
   (
      .x (a),
      .z (c)
   );

   my_mod2 my_mod2_inst
   (
      .y (b),
      .z (c)
   );

   my_mod3 my_mod3_inst
   (
      .z (c)
   );

   my_mod4 my_mod4_inst ();

   my_mod5 my_mod5_inst ();

endmodule

module my_mod0(x, y, z);
   input wire [7:0] x;
   input wire [7:0] y;
   output wire [7:0] z;

   always @(*) begin
      $display("x=%0d, y=%0d", x, y);
   end
endmodule

module my_mod1(x, y, z);
   input wire [7:0] x;
   output wire [7:0] y;
   input wire [7:0] z;

   always @(*) begin
      $display("x=%0d, z=%0d", x, z);
   end
endmodule

module my_mod2(x, y, z);
   output wire [7:0] x;
   input wire [7:0] y;
   input wire [7:0] z;

   always @(*) begin
      $display("y=%0d, z=%0d", y, z);
   end
endmodule

module my_mod3(x, y, z);
   output wire [7:0] x;
   output wire [7:0] y;
   input wire [7:0] z;

   always @(*) begin
      $display("z=%0d", z);
   end
endmodule

module my_mod4(x, y, z);
   output wire [7:0] x;
   output wire [7:0] y;
   output wire [7:0] z;

   initial begin
      $display("nothing");
   end
endmodule

module my_mod5();
   initial begin
      $display("no ports in the module");
   end
endmodule

module testbench();
   instance7 inst7 (8'd5, 8'd6, 8'd7);
endmodule
