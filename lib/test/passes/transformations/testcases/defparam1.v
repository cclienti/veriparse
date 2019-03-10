module parametrized();
   parameter X=0;

   initial begin
      $display("X=%0d", X);
   end

endmodule

module defparam1;
   genvar i;

   generate
      for (i=0; i<5; i=i+1) begin: SCOPE
         parametrized modinst();
      end
   endgenerate

   generate
      for (i=0; i<5; i=i+1) begin
         defparam SCOPE[i].modinst.X = i;
      end
   endgenerate

endmodule
