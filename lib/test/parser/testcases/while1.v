module while1();

   integer i;
   reg [7:0] ram [0:255];

   initial begin
      i=0;
      while(i<256) ram[i] = 2*i+1;
   end

endmodule
