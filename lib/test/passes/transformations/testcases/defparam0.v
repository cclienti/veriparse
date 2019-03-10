module parametrized;
   parameter X=0;

   initial begin
      $display("X=%0d", X);
   end

endmodule

module defparam0;
   parametrized modinst2();
   defparam modinst2.X = 2;
endmodule
