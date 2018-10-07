module case2();

   initial begin
      case(2'b00)
         2'b00: begin
            case(4)
               0: $display("0");
               1: $display("1");
               2: $display("2");
               3: $display("3");
               default: $display("4");
            endcase
         end

         2'b01: begin
            case(0)
               0: $display("a");
               1: $display("b");
               2: $display("c");
               3: $display("d");
               default: $display("e");
            endcase
         end

         default: begin
            $display("default");
         end
      endcase
   end

endmodule
