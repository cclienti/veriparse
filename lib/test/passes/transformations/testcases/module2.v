module module2
  #(parameter WIDTH      = 2**LOG2_WIDTH,
    parameter LOG2_WIDTH = 3,
    parameter LOG2)
   (clock, reset_n, in, out, strobe);

   input clock;
   input reset_n;
   input wire [LOG2_WIDTH:0] in;
   output [WIDTH-1:0] out;
   output strobe;
   reg [WIDTH-1:0] out;
   reg strobe;
   reg [LOG2-1:0] l;

   integer j;
   reg [WIDTH-1:0] lut [WIDTH-1:0];

   initial begin
      for(j=0; j<WIDTH-1; j=j+1) begin
         lut[j] = j+1;
      end
   end

   always @(negedge reset_n or posedge clock) begin
      l <= in;
      out <= lut[l];
   end

endmodule
