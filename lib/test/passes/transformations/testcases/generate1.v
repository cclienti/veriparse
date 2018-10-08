module generate1;

   generate
      integer x, y;
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

   generate
      case(3)
         0: initial $display("0");
         1: initial $display("1");
         2: initial $display("2");
         3: initial $display("3");
      endcase
   endgenerate

   generate
      case(3)
         0: initial $display("0");
         1: initial $display("1");
         2: initial $display("2");
         3: initial $display("3");
      endcase
   endgenerate

   generate
      genvar i;
      for (i = 0; i < 10; i = i + 1) begin: gen1
         genvar j;
         for (j = i; j >= 1; j = j - 1) begin: gen2
            reg [i:0] R;
            initial begin
               R = i;
               $display("%m", R);
            end
         end
      end
   endgenerate

endmodule
