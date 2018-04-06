module event1 (clk);
   input wire       clk;
   integer i;

   initial begin
      i=0;
      while(1) begin
         @(posedge clock) i <= i+1;
         if(i == 256) $finish;
      end
   end

endmodule
