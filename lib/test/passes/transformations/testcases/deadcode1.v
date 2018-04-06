module deadcode1
  #(parameter width = 32)
   (input wire clock,
    input wire reset,
    input wire [7:0] in,
    output reg [width-1:0] out);

   reg [4:0] c, e;
   reg [7:0] debug0;
   reg [7:0] debug1;
   reg [7:0] debug2;
   reg [7:0] [7:0] debug3;
   reg [7:0] debug4;
   reg [7:0] debug5;

   integer i, j;
   wire [4:0] f = e;
   wire [4:0] d = c;
   assign e = width>>1;

   always @(posedge clock) begin
      if(reset) begin
         out <= 0;
         debug0 <= 0;
      end
      else begin
         if (in == 0) begin
            debug0 <= in;
         end
         else begin
            out [e +: 8] <= in;
            debug0 <= in;
         end
      end
   end

   always @(posedge clock) begin
      debug5 <= in;
      if (in[0] == 0) begin
         debug4 <= in;
      end
      $display("debug5 = %x", debug5);
   end

   always @(posedge clock) begin
      for(i=0; i<8; i=i+1) debug1[i] <= in[i];
      for(i=0; i<8; i=i+1) begin
         debug2[i] <= in[i];
      end
   end

   always @(posedge clock) begin
      i=0;
      while(i<8) begin
         j=0;
         while(j<8) begin
            debug3[i][j] <= in[i];
            j=j+1;
         end
         i=i+1;
      end
   end

endmodule
