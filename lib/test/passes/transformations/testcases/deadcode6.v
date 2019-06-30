module mod1();
   wire [7:0] x;

   always @(*) begin
      $display("x=%0h", x);
   end

endmodule

module deadcode6();
   mod1 mod1_inst();
   assign mod1_inst.x = 8'h42;
endmodule
