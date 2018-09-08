module instance6(a, b, c);
   input wire [7:0] a;
   input wire [7:0] b;
   input wire [7:0] c;

   my_mod inst0 (a, b, b/*not used*/);
   my_mod inst1 (b, c, c/*not used*/);

endmodule

module my_mod(x, y, z);
   input wire [7:0] x;
   input wire [7:0] y;
   input wire [7:0] z;

   always @(*) begin
      $display("x=%0d, y=%0d", x, y);
   end
endmodule

module testbench();
   instance6 inst6 (8'd5, 8'd6, 8'd7);
endmodule
