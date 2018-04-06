module fork0(clk, a, b);
   input clk;
   output a;
   output b;

   reg a, b;

   initial begin
      a = 0;
      b = 0;
   end

   always @(posedge clk) fork
      #2 a = 1;
      #1 b = 1;
   join

endmodule
