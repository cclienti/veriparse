module case0();

   initial begin
      case(3)
         0, 4: $display("0");
         1, 5: $display("1");
         2, 6: $display("2");
         3, 7: $display("3");
      endcase
   end

   initial begin
      case(4)
         0, 4:    $display("0 or 4");
         1:       $display("1");
         2:       $display("2");
         3:       $display("3 or 4");
         default: $display("4");
      endcase
   end

   initial begin
      case(4)
         0: $display("0");
         1: $display("1");
         2: $display("2");
         3: $display("3");
         default: $display("4");
      endcase
   end

   initial begin
      case(1)
         0: begin
            $display("0");
         end

         1: begin
            $display("1");
         end

         2: begin
            $display("2");
         end

         3: begin
            $display("3");
         end
      endcase
   end

   initial begin
      case(2)
         0: begin: ZERO
            $display("0");
         end

         1: begin: ONE
            $display("1");
         end

         2: begin: TWO
            $display("2");
         end

         3: begin: THREE
            $display("3");
         end
      endcase
   end

endmodule
