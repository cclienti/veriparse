module block1();

   initial begin
      begin
      end
   end

   initial begin: BLOCKA
      begin
      end
   end

   initial begin
      begin: BLOCKB
      end
   end

   generate
      begin
         begin
         end
      end
   endgenerate

   generate
      begin: BLOCK0
         begin: BLOCK1
         end
      end
   endgenerate

endmodule
