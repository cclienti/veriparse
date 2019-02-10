module initial_test0;

   initial if (5 < 3) begin
      $display("impossible");
   end

   initial begin
      if (5 < 3) begin
         $display("impossible");
      end
   end

   initial if (2 < 3) begin
      $display("ok");
   end

   initial begin
      if (2 < 3) begin
         $display("ok");
      end
   end

endmodule
