/* -----\/----- EXCLUDED -----\/-----
module my_module(clock, a, b, c, d, e);
   input wire clock;
   input wire [7:0] a;
   input wire [1:0] b;
   input wire [1:0] c;
   output reg d, e;

   always @(posedge clock) begin
      d <= (a[0] || b[0]) && (a[1] || b[1]) || c[1];
      e <= (a[1] || b[0]) && (a[0] || b[1]) || c[0];
   end
endmodule
 -----/\----- EXCLUDED -----/\----- */

module instance5(clock, a0, a1, b, c, d);
   /* verilator lint_off LITENDIAN */
   input wire clock;
   input wire [3:0] [7:0] a;
   input wire [0:7] b;
   input wire [7:0] c;
   output reg [3:0] d;

   reg [3:0] [7:0] x [2:0] [1:0];

   assign x[0] = a0;
   assign x[1] = a1;

   my_module inst [3:0]
     (.clock(clock), .a(a), .b(b), .c(c),
      .d(d), .e());

endmodule
