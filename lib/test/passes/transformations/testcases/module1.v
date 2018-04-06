module module1 (clock, reset_n, in, out, strobe);
   parameter WIDTH = 2**LOG2_WIDTH;
   parameter LOG2_WIDTH = 3;
   parameter [1:0] VECTOR_0 = 2'd0,
     VECTOR_1 = 2'd0,
     VECTOR_2 = 2'd1,
     VECTOR_3 = 2'd2;
   parameter LOG2 = $clog2(WIDTH);

   input clock;
   input reset_n;
   input wire [LOG2_WIDTH:0] in;
   output [WIDTH-1:0] out;
   output strobe;
   reg [WIDTH-1:0] out;
   reg strobe;

   integer j;
   reg [WIDTH-1:0] lut [WIDTH-1:0];

   initial begin
      for(j=0; j<WIDTH-1; j=j+1) begin
         lut[j] = j+1;
      end
   end

   always @(negedge reset_n or posedge clock) begin
      out <= lut[in];
   end

endmodule
