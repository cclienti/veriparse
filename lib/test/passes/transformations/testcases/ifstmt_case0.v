module ifstmt_case0();

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
               case(3)
                  0: $display("-4");
                  default: begin
                     $display("4");
                     if (5) begin
                        $display("5");
                     end
                     else begin
                        $display("-5");
                     end
                  end
               endcase
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
