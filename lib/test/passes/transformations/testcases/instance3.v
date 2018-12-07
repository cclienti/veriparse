module instance3 (a, b, c);
   input [3:0]  a, b;
   output [3:0] c;
   my_module3 inst [3:0] (a, b, c);
endmodule

module my_module3 (ia, ib, oc);
   input wire ia, ib;
   output reg oc;

   always @(*) oc = ia && ib;
endmodule

module testbench;

   reg clk = 0;
   always
     #5 clk = ~clk;

   integer cpt = 0;

   reg [3:0] a = 0;
   reg [3:0] b = 0;
   wire [3:0] c;

   instance3 instance3_inst
   (
      .a (a),
      .b (b),
      .c (c)
   );

   always @(posedge clk) begin
      cpt <= cpt + 1;
      a <= cpt[3:0];
      b <= cpt[4:1];
      if (cpt == 7)
        $finish;
   end


   always @(*) begin
      $display(a, b, c);
   end

endmodule
