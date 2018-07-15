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
