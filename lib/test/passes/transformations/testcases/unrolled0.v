module unrolled0
  (input wire [3:0] in,
   output reg [7:0] out);

   integer j, k, offset;
   reg [7:0] lut [15:0];

   initial begin
      k=0+1;
      offset=k+1;
      lut[k-1]=2*offset;

      k=1+1;
      offset=k+1;
      lut[k-1]=2*offset;

      k=2+1;
      offset=k+1;
      lut[k-1]=2*offset;

      k=3+1;
      offset=k+1;
      lut[k-1]=2*offset;

      k=4+1;
      offset=k+1;
      lut[k-1]=2*offset;

      k=5+1;
      offset=k+1;
      lut[k-1]=2*offset;

      k=6+1;
      offset=k+1;
      lut[k-1]=2*offset;

      k=7+1;
      offset=k+1;
      lut[k-1]=2*offset;

      k=8+1;
      offset=k+1;
      lut[k-1]=2*offset;

      k=9+1;
      offset=k+1;
      lut[k-1]=2*offset;

      k=10+1;
      offset=k+1;
      lut[k-1]=2*offset;

      k=11+1;
      offset=k+1;
      lut[k-1]=2*offset;

      k=12+1;
      offset=k+1;
      lut[k-1]=2*offset;

      k=13+1;
      offset=k+1;
      lut[k-1]=2*offset;

      k=14+1;
      offset=k+1;
      lut[k-1]=2*offset;

      k=15+1;
      offset=k+1;
      lut[k-1]=2*offset;
   end

   always @(*) begin
      out=lut[in];
   end


endmodule
