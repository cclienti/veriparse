module pragma2(reset, clk, in, out);
   input wire       reset, clk;
   input wire [7:0] in;
   output reg [15:0] out;

   always @(*) begin
      (* full_case, other = 3 *)
      case (in)
         0: out = {8'd0, in};
         1: out = 2*in;
         default: out = 2*in - 1;
      endcase
   end

   always @(*) (* pragma *) $display("out=%d", out);

endmodule
