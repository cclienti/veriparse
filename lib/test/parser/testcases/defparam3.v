module defparm3;
   defparam my_proxy_inst.my_module_inst.VALUE_0 = 32;
   defparam my_proxy_inst.my_module_inst.VALUE_1 = 33;
   my_proxy my_proxy_inst();
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
