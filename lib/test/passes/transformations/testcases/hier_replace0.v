module hier_replace0;

  wire [7:0] a;
  wire [7:0] b;
  wire [7:0] c;

  // Plain WIDTH is substituted; the hierarchical u1.WIDTH denotes another
  // scope's name and must stay; the label index expression WIDTH inside
  // u2[WIDTH].sig is a plain local reference and is substituted.
  assign a = WIDTH;
  assign b = u1.WIDTH;
  assign c = u2[WIDTH].sig;

endmodule
