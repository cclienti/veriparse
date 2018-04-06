module lvalue5(a, b);
   input reg [63:0]  a;
   output reg [63:0] b;

   integer i, j;
   reg [7:0] [7:0] tmp;

   always @* begin
      for(j=0; j<8; j=j+1) begin
         for(i=1; i<8; i=i+2) begin
            tmp[j][i -: 2] = a[i -: 2];
         end
      end
   end

   always @* begin
      b = tmp;
   end

endmodule
