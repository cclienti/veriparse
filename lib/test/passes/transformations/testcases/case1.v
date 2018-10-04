module case1();

   generate
      case(3)
         0: initial $display("0");
         1: initial $display("1");
         2: initial $display("2");
         3: initial $display("3");
      endcase
   endgenerate

   generate
      case(4)
         0: initial $display("0");
         1: initial $display("1");
         2: initial $display("2");
         3: initial $display("3");
         default: initial $display("4");
      endcase
   endgenerate

   generate
      case(2)
         0: begin
            initial $display("0");
         end

         1: begin
            initial $display("1");
         end

         2: begin
            initial $display("2");
         end

         3: begin
            initial $display("3");
         end
      endcase
   endgenerate

   generate
      case(1)
         0: begin: ZERO
            initial $display("0");
         end

         1: begin: ONE
            initial $display("1");
         end

         2: begin: TWO
            initial $display("2");
         end

         3: begin: THREE
            initial $display("3");
         end
      endcase
   endgenerate


   generate
      case(0)
         0: begin: ZERO
            initial begin
               $display("0");
            end
         end

         1: begin: ONE
            initial begin
               $display("1");
            end
         end

         2: begin: TWO
            initial begin
               $display("2");
            end
         end

         3: begin: THREE
            initial begin
               $display("3");
            end
         end
      endcase
   endgenerate

endmodule
