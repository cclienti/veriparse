module instance1;
   localparam L_INPUT_WIDTH  = 8;
   localparam L_OUTPUT_WIDTH = 8;

   wire       clock;
   wire       reset;
   wire [7:0] in0_i;
   wire [7:0] in1_i;
   reg [7:0]  out_i;

   mod #(.INPUT_WIDTH  (L_INPUT_WIDTH),
         .OUTPUT_WIDTH (L_OUTPUT_WIDTH))
   mod_inst (.clock (clock),
             .reset (reset),
             .in0   (in0_i),
             .in1   (in1_i),
             .out   (out_i));
endmodule
