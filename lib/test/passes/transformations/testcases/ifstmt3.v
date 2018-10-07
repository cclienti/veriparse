module ifstmt3();

   initial begin
      if (1 == 1) begin
         $display("1");
         if (2 == 2) begin
            $display("2");
            if (3 == 2) begin
               $display("-3");
            end
            else begin
               $display("3");
            end
         end
         else begin
            $display("-2");
         end
      end
      else begin
         $display("-1");
      end
   end

endmodule
