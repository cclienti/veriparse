module ifstmt2 (output reg [31:0] a);

   integer x, y, z;

   initial begin
      x = 1;
      y = 0;

      if(x == 1) begin
         y = x + 3;
      end

      if(x == 1) begin
         if(y == 2) begin
            z = x;
         end
         else begin
            z = x;
         end
         z = x + 1;
      end
      else begin
         z = x + 2;
      end

      a = z;
   end

   always @(*) begin
      $display(a);
   end

endmodule
