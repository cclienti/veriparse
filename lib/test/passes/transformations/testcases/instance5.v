module instance5
  (input wire             clock,
   input wire [3:0] [7:0] a,
   input wire [0:7]       b,
   input wire [7:0]       c,
   output wire [0:3]      d);

  my_module inst [3:0]
    (.clock (clock),
     .a     (a),
     .b     (b),
     .c     (c),
     .d     (d),
     .e     ());

endmodule

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

module instance5_testbench();
   //----------------------------------------------------------------
   // Signals
   //----------------------------------------------------------------

   reg            clock;
   reg [3:0][7:0] a;
   reg [0:7]      b;
   reg [7:0]      c;
   wire [0:3]     d;

   integer        cpt;


   //----------------------------------------------------------------
   // Clock generation
   //----------------------------------------------------------------

   always begin
      clock = 0;
      #5 clock = !clock;
   end


   //----------------------------------------------------------------
   // DUT
   //----------------------------------------------------------------

   instance5 instance5_inst
   (
      .clock (clock),
      .a     (a),
      .b     (b),
      .c     (c),
      .d     (d)
   );


   //----------------------------------------------------------------
   // Test vectors
   //----------------------------------------------------------------

   initial cpt = 0;

   always @(posedge clock) begin
      cpt <= cpt + 1;
      $display(d);
   end

   always @(cpt) begin
      case(cpt)
         1: begin
            a = {8'b1010_1100, 8'b1001_0110, 8'b1111_0001, 8'b1010_0101};
            b = 8'b1110_0111;
            c = 8'b0011_1100;
         end
         5: begin
            $finish;
         end
         default: begin
            a = {8'b0000_0000, 8'b0000_0000, 8'b0000_0000, 8'b0000_0000};
            b = 8'b0000_0000;
            c = 8'b0000_0000;
        end
      endcase
   end


endmodule
