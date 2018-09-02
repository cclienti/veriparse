module instance0;
   reg [11*8:1] str = "Hello World";
   string_test_mod disp [10:0] (.car(str));

   reg [8:1] test_ab = "1";
   string_test_mod disp_a [0:0] (.car(test_ab));
   string_test_mod disp_b [3:0] (.car(test_ab));

   reg [16:1] test_c = "22";
   string_test_mod disp_test3 [1:0] (.car(test_c));

endmodule

module string_test_mod (input wire [7:0] car);
   always @(car) begin
      $display("%c", car);
      $finish;
   end
endmodule
