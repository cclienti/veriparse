module vf_loop_jump_trunc0;

   integer i, y, z;

   initial begin
      y = 0;
      i = 0;
      while (1) begin
         i = i + 1;
         if (i == 2) begin
            break;
            y = 1; // unreachable: must not leak into the unrolled output
         end
      end
      z = y;
   end

endmodule
