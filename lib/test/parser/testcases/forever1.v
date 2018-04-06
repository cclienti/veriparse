module forever_example ();

   reg clk;

   initial begin
      #1 clk = 0;
      forever #5 clk = !clk;
   end

   initial begin
      $monitor ("Time = %d  clk = %b",$time, clk);
      #100 $finish;
   end

endmodule
