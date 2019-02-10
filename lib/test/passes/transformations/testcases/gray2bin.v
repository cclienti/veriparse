`timescale 1 ns / 100 ps

module gray2bin
  (input wire [3:0]  gray,
   output wire [3:0] bin);


   reg [4-1:0] lut_array [15:0];
   reg [4-1:0] bin_temp;
   reg [4-1:0] gray_temp;

   integer i;

   initial begin
      for(i=0 ; i<16 ; i=i+1) begin
         bin_temp              = i[3:0];
         gray_temp             = (bin_temp >> 1) ^ bin_temp;
         lut_array[gray_temp]  = bin_temp;
      end
   end

   assign bin = lut_array[gray];

endmodule


module testbench;

   reg [4:0] gray;
   wire [3:0] bin;
   reg clk;

   initial begin
      clk = 0;
      gray = 0;
   end

   always begin
      #4 clk = !clk;
   end

   gray2bin DUT (.gray(gray[3:0]), .bin(bin));

   always @(posedge clk) begin
      gray <= gray + 1;
   end

   always @(*) begin
      if (gray[4]) begin
         $finish;
      end

      $display("gray: %04b - bin: %04b", gray[3:0], bin);
   end

endmodule
