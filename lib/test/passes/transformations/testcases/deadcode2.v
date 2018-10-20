`timescale 1 ns / 100 ps

module deadcode2
  #(parameter WIDTH = 8)
   (input wire clock,
    input wire [WIDTH-1:0] in0, in1,
    output reg [WIDTH-1:0] out);

   reg [WIDTH-1:0] out_comb;
   reg [WIDTH-1:0] out_comb2;
   reg [WIDTH-1:0] out_comb3;

   task print_value(input [WIDTH-1:0] out);
      begin
         $display ("%0d + %0d = %0d (%0d)", in0, in1, out, out_comb2);
      end
   endtask

   always @(*) begin
      out_comb = in0 + in1;
      print_value(out_comb);
      out_comb2 = out_comb;
      out_comb3 = out_comb;
   end

   always @(posedge clock) begin
      out <= out_comb;
   end

endmodule


module deadcode2_tb;
   localparam WIDTH = 8;

   reg clock;
   reg [WIDTH-1:0] in0;
   reg [WIDTH-1:0] in1;
   wire [WIDTH-1:0] out;

   initial begin
      clock = 0;
      in0 = 1;
      in1 = 2;
      #100 $finish;
   end

   always
     #5 clock = ~clock;

   always @(posedge clock) begin
      in0 <= in0 + 1;
      in1 <= in1 + 1;
   end

   deadcode2
   #(
      .WIDTH (WIDTH)
   )
   decode2_inst
   (
      .clock (clock),
      .in0   (in0),
      .in1   (in1),
      .out   (out)
   );

endmodule
