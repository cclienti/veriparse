module while0();

   integer i, j;
   reg [7:0] ram [0:255] [0:7];

   initial begin
      i=0;
      j=0;
      while(i<256) begin
        while(i<8) begin
         ram[i][j] = i*j+1;
         $display("Address = [%g][%g]  Data = %h", i, j, ram[i][j]);
        end
      end
      #1 $finish;
   end

endmodule
