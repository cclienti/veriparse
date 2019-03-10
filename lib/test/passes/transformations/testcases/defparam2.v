module parametrized();
   parameter X=0;

   initial begin
      $display("X=%0d", X);
   end

endmodule

module forward();
   parametrized pinst();
endmodule

module defparam2;
   genvar i;

   generate
      for (i=0; i<5; i=i+1) begin: SCOPE
         forward finst();
      end
   endgenerate

   generate
      for (i=0; i<5; i=i+1) begin
         defparam SCOPE[i].finst.pinst.X = i;
      end
   endgenerate

endmodule
