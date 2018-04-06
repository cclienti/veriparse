module generate2
  #(parameter CONFIG = 0,
    parameter WIDTH = 4)
   (input wire clock,
    input wire [WIDTH-1:0] in0, in1,
    output reg [WIDTH-1:0] out);

   genvar i;

   generate
      if (CONFIG == 0) begin
         for (i=0; i < WIDTH; i=i+1) begin: LOOP0
            always @(posedge clock) out[i] <= in0[i] || in1[i];
         end
      end
      else begin
         for (i=0; i < WIDTH; i=i+1) begin: LOOP1
            always @(posedge clock) out[i] <= in0[i] && in1[i];
         end
      end
   endgenerate

endmodule
