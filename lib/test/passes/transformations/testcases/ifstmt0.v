module ifstmt0 (output reg [31:0] a);

   integer x, y;

   initial begin
      if (0) x = 1;
      else x = 4;

      if (1'b1) begin
         y = 2;
      end
      else begin
         y = 3;
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
