module defparm2;
   defparam
     my_proxy_inst.my_module_inst.VALUE_0 = 32,
     my_proxy_inst.my_module_inst.VALUE_1 = 33,
     another_module_inst.OTHER = 15;

   my_proxy my_proxy_inst();
   another_module another_module_inst();
endmodule

module my_proxy;
   my_module #(7) my_module_inst();
endmodule

module my_module #(parameter VALUE_0 = 5,
                   parameter VALUE_1 = 9);
  initial begin
     $display("VALUE_0=%0d", VALUE_0);
     $display("VALUE_1=%0d", VALUE_1);
  end
endmodule

module another_module #(parameter OTHER = 1);
   initial begin
      $display("OTHER=%0d", OTHER);
   end
endmodule
