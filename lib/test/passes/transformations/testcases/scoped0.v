module mod1;
   wire [7:0] x;
   wire [7:0] y;

   mod2 inst(.x(x));

   assign inst.y = 8'h45;

   always @(*) begin
      $display("mod1 y=%0h", y);
   end
endmodule

module mod2(input wire [7:0] x);
   wire [7:0] z;
   wire [7:0] y;
   always @(*) begin
      $display("mod2 x=%0h", x);
      $display("mod2 z=%0h", z);
      $display("mod2 y=%0h", y);
   end
endmodule

module scoped0;
   mod1 inst();

   always @(*) begin
      $display("mod1_inst.mod2_inst.x=%0h", inst.inst.x);
   end

   assign inst.x = 8'h42;
   assign inst.y = 8'h43;
   assign inst.inst.z = 8'h44;

endmodule
