module sv_deadcode_jump0
  (input c, input [3:0] n, output reg [7:0] o);

   integer i;

   always @* begin
      o = 0;
      for (i = 0; i < 4; i = i + 1) begin
         if (c) continue;      // conditional: siblings survive
         o = o + 1;
         break;                // unconditional: the rest is dead
         o = o + 100;
         o = o + 200;
      end
   end

endmodule
