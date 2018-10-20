`timescale 1 ns / 100 ps

module deadcode3
  #(parameter WIDTH = 8)
   (input wire clock,
    input wire [WIDTH-1:0] in0, in1,
    output reg [WIDTH-1:0] out);

   reg [WIDTH-1:0] out_comb;
   reg [WIDTH-1:0] out_comb2;
   reg [WIDTH-1:0] out_comb3;

   function [WIDTH-1:0] adder(input [WIDTH-1:0] in0, in1);
      begin
         adder = in0 + in1;
         if (out_comb2 !== 'hx) begin
            $display("%0d + %0d = %0d", in0, in1, adder);
         end
      end
   endfunction

   always @(*) begin
      out_comb = adder(in0, in1);
      out_comb2 = out_comb;
      out_comb3 = out_comb;
   end

   always @(posedge clock) begin
      out <= out_comb;
   end

endmodule


module deadcode3_tb;
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

   deadcode3
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
