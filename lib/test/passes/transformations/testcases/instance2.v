module instance2;
   localparam L_INPUT_WIDTH  = 8;
   localparam L_OUTPUT_WIDTH = 8;

   wire       clock;
   wire       reset;
   wire [7:0] in0_i;
   wire [7:0] in1_i;
   reg [7:0]  out_i;

   mod #(L_INPUT_WIDTH,L_OUTPUT_WIDTH)
   mod_inst (clock, reset, in0_i, in1_i, out_i);

endmodule
