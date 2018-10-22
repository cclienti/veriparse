module defparm1;
   defparam my_proxy_inst.my_module_inst.VALUE = 32;
   my_proxy my_proxy_inst();
endmodule

module my_proxy;
   my_module #(7) my_module_inst();
endmodule

module my_module #(parameter VALUE=5);
  initial begin
     $display("VALUE=%0d", VALUE);
  end
endmodule
