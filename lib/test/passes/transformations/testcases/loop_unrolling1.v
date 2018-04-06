module loop_unrolling1();

   integer i, j;
   reg [7:0] ram [0:15][0:7];

   initial
     for (i=0; i<15; i=i+1) begin
       for (j=0; j<8; j=j+1)
          ram[i][j] = i*j+1;
     end


endmodule
