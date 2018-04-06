module lvalue2(a, b0, b1, c);
   output reg [63:0] a;
   input reg [3:0] [4:0] b0;
   input reg [3:0] [5:0] b1;
   input reg [1:0] c;

   always @* begin
      {a[b0[c-1] -: 8],  a[b1[c] +: 8]} = 0;
   end

endmodule
