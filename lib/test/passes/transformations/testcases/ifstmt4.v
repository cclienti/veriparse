module ifstmt4 (output reg [31:0] a);

   integer x, y, z;

   function integer equal(input integer a, input integer b);
      begin
         equal = (a == b);
      end
   endfunction

   initial begin
      x = 1;
      y = 0;

      if (equal(1, 1)) begin
         y = x + 3;
      end

      if (equal(1, 1)) begin
         if (equal(0, 2)) begin
            z = x + y;
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
      $display(a);
   end

endmodule
