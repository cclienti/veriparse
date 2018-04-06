module instance0;
   wire       clock;
   wire       reset;
   wire [7:0] in0_i;
   wire [7:0] in1_i;
   reg [7:0]  out_i;

   mod mod_inst
   (
      .clock (clock),
      .reset (reset),
      .in0   ({in0_i[7:4], in0_i[3:0]}),
      .in1   (in1_i),
      .out   (out_i)
   );

endmodule


module mod (input wire clock,
            input wire reset,
            input wire [7:0] in0, in1,
            output reg [7:0] out);

   always @(posedge clock) begin
      if (reset) begin
         out <= 0;
      end
      else begin
         out <= in0 + in1;
      end
   end

endmodule
