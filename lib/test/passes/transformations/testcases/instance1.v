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


module mod
  #(parameter INPUT_WIDTH = 8,
    parameter OUTPUT_WIDTH = 8)

   (input wire                    clock,
    input wire                    reset,
    input wire [INPUT_WIDTH-1:0]  in0, in1,
    output reg [OUTPUT_WIDTH-1:0] out);

   wire [OUTPUT_WIDTH-1:0] in0_ex, in1_ex;

   assign in0_ex = {{INPUT_WIDTH{1'b0}}, in0};
   assign in1_ex = {{INPUT_WIDTH{1'b0}}, in1};

   always @(posedge clock) begin
      if (reset) begin
         out <= 0;
      end
      else begin
         out <= in0_ex * in1_ex;
      end
   end

endmodule
