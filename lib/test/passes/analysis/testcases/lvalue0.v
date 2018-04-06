module lvalue0(a, b, c);
   output reg [31:0] a;
   input reg [3:0] [4:0] b;
   input reg [1:0] c;

   assign a[b[c]] = 0;

endmodule
