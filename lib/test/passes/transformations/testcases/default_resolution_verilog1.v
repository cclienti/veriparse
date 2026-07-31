// Verilog-2005 has no `static` keyword for subroutines (1364-2005 A.2.6 is
// `task [automatic]`), so resolving an effective lifetime must not stamp one
// in Verilog mode: the pass promises 1364 in, 1364 out.
module default_resolution_verilog1 (c, q);
   input c;
   output [3:0] q;
   reg [3:0] r;

   task t;
      input [3:0] u;
      output [3:0] o;
      begin
         o = ~u;
      end
   endtask

   function [3:0] f;
      input [3:0] u;
      begin
         f = u + 4'd1;
      end
   endfunction

   always @(c) begin
      t({3'b000, c}, r);
   end

   assign q = f(r);
endmodule
