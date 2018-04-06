module namedblock0();
   reg clk,reset,enable,data;

   initial begin: INIT
      begin: CLOCK_RESET
         clk = 0;
         reset = 0;
      end
      begin: BUS
         enable = 0;
         data = 0;
      end
   end

endmodule
