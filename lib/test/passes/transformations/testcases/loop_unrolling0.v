module loop_unrolling0();

   integer i, j;
   reg [7:0] ram [0:15][0:7];

   initial begin
      for (i=0; i<15; i=i+1)
        for (j=0; j<8; j=j+1) begin
           ram[i][j] = i*j+1;
        end
   end

endmodule
