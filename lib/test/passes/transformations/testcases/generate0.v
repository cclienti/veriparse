module generate0;

   generate
      initial
        $display("5");
   endgenerate

   generate
      initial begin
         $display("4");
      end
   endgenerate

   generate
      integer a=1;
      always @(*) begin
         if (a) begin
            $display("3");
         end
      end
   endgenerate

   generate
      integer b=2;
      always @(*) begin
         case(b)
            2: $display("2");
         endcase
      end
   endgenerate

   generate
      integer c=3;
      always @(*) begin
         casez(c)
            3: $display("1");
         endcase
      end
   endgenerate

   generate
      integer d=4;
      always @(*) begin
         casex(d)
            1: $display("0");
         endcase
      end
   endgenerate

endmodule
