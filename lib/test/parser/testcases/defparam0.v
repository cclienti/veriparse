module defparm0;

   wire clock;
   wire [31:0] in0;
   wire [31:0] out;

   defparam myregister_inst.WIDTH = 32;

   myregister myregister_inst (.clock (clock),
                               .in0   (in0),
                               .out   (out));
endmodule

module myregister
  #(parameter WIDTH = 2)
   (input wire clock,
    input wire [WIDTH-1:0] in0,
    output reg [WIDTH-1:0] out);

   always @(posedge clock) begin
      out <= in0;
   end

endmodule
