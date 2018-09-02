module instance1;
   string_test_mod disp [10:0] (.car("Hello World"));
   string_test_mod disp2 [1:0] (.car("1"));
   string_test_mod disp3 [1:0] (.car(""));
   string_test_mod disp4 [1:0] (.car("22"));
endmodule

module string_test_mod (input wire [7:0] car);
   always @(car) begin
      $display("%c", car);
      $finish;
   end
endmodule
