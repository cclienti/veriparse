module instance5
  (input wire             clock,
   input wire [3:0] [7:0] a,
   input wire [0:7]       b,
   input wire [7:0]       c,
   output wire [0:3]      d);

  my_module inst [3:0]
    (.clock (clock),
     .a     (a),
     .b     (b),
     .c     (c),
     .d     (d),
     .e     ());

endmodule
