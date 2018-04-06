module event0 (clk);
   input wire       clk;
   integer i;

   initial begin
      i=0;
      while(1) begin
         @(posedge clock) begin
            i <= i+1;
            $display("clock cycle %d", i);
         end
         if(i == 256) $finish;
      end
   end

endmodule
