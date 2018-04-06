module assignment1(input wire  clock,
                   input wire  a, b,
                   output wire s);
   always @(posedge clock) s <= a & b;
endmodule
