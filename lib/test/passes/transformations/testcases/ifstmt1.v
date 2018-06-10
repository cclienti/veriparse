module ifstmt0 (output reg [31:0] a);
   parameter Z = 0;

   integer x, y;

   initial begin
      if(Z == 0) begin
         if (0) x = 1;
         else x = 4;

         if (1'b1) begin
            y = 2;
         end
         else begin
            y = 3;
         end
      end
      else begin
         if (1) x = 1;
         else x = 4;

         if (0) begin
            y = 2;
         end
         else begin
            y = 3;
         end
      end
   end

   generate
      if (1) begin
   always @(*) begin
            a = x + y;
   end
      end
      else begin
         always @(*) begin
            a = x * y;
         end
      end
   endgenerate

   always @(*) begin
      $display(a);
   end

endmodule
