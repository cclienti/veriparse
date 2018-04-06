module for1();

   integer i, j;
   reg [7:0] ram [0:255][0:7];

   initial begin
      for (i=0; i<256; i=i+1)
        for (j=0; j<8; j=j+1) ram[i][j] = i*j+1; // Initialize the RAM with 0
      #1 $finish;
   end

endmodule
