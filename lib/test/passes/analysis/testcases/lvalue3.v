module lvalue3(a, b);
   input reg [63:0]  a;
   output reg [63:0] b;

   integer i, j;
   reg [7:0] [7:0] tmp;

   always @* begin
      for(i=0; i<8; i=i+1) begin
         for(j=0; j<8; j=j+1) begin
            tmp[j][i] = a[i*8+j];
         end
      end
   end

   always @* begin
      b = tmp;
   end

endmodule
